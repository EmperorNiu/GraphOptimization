//
// Created by lenovo on 2022/3/10.
//
#include "optimizer_graph_hierarchical.h"
#include <algorithm>
#include "../utils/util.h"

vector<vector<vector<tuple<float,pair<int,int>,int>>>> compute_partitioning(vector<vector<float>> compute_times,
                                                                            vector<vector<float>> activation_sizes,
                                                                            vector<vector<float>> parameter_sizes,
                                                                            vector<float> output_activation_sizes,
                                                                            vector<vector<int>> all_predecessor_ids,
                                                                            int num_machines,
                                                                            int num_machines_within_machine,
                                                                            int bandwidth, long memory_size,
                                                                            bool straight_pipeline, bool final_level) {
    // tuple: min_pipeline_time, optimal_splits(start, end), optimal_num_machines
    // initialization A
    vector<vector<vector<tuple<float,pair<int,int>,int>>>> A;
    for (int i = 0; i < compute_times.size(); ++i) {
        vector<vector<tuple<float,pair<int,int>,int>>> row_A;
        for (int j = 0; j < compute_times[0].size(); ++j) {
            vector<tuple<float,pair<int,int>,int>> row_row_A;
            for (int k = 0; k < num_machines; ++k) {
                row_row_A.push_back(tuple<float,tuple<int,int>,int>(-1.0,make_pair(-1,-1),-1));
            }
            row_A.push_back(row_row_A);
        }
        A.push_back(row_A);
    }
    for (int i = 0; i < compute_times.size(); ++i) {
        for (int j = i; j < compute_times[0].size(); ++j) {
            float cum_compute_time = compute_times[i][j];
            float cum_activation_size = activation_sizes[i][j];
            float cum_parameter_size = parameter_sizes[i][j];
            int max_m = straight_pipeline ? 1 : num_machines;
            for (int k = 0; k < max_m; ++k) {
                float stashed_data_size = ceil((num_machines - (k+1)) / (k+1))
                                        * (cum_activation_size + cum_parameter_size);
                // memory constraint
                if (stashed_data_size > memory_size)
                    continue;
                float data_parallel_communication_time = ((4 * k * cum_parameter_size) / (bandwidth * (k + 1))) /
                                                            num_machines_within_machine;
                if (cum_compute_time == -1) {
                    A[i][j][k] = tuple<float,tuple<int,int>,int>(-1.0,make_pair(-1,-1),-1);
                } else {
                    A[i][j][k] = tuple<float,tuple<int,int>,int>((cum_compute_time+data_parallel_communication_time)/(k+1),
                                                                 make_pair(-1,-1),k+1);
                }
            }
        }
    }
    int min_machines = 1;
    int max_i = !final_level ? compute_times.size() : 1;
    for (int i = 0; i < max_i; ++i) {
        for (int m = min_machines; m < num_machines; ++m) {
            for (int j = i+1; j < compute_times[0].size(); ++j) {
                float min_pipeline_time = get<0>(A[i][j][m]);
                pair<int,int> optimal_split = get<1>(A[i][j][m]);
                int optimal_num_machines = get<2>(A[i][j][m]);
                // use fewer machine ? if ()
//                float tmp = get<0>(A[i][j][m-1]);
//                if (m > 0 && (min_pipeline_time == -1 || tmp < min_pipeline_time)) {
//                    min_pipeline_time = get<0>(A[i][j][m-1]);
//                    optimal_split = get<1>(A[i][j][m-1]);
//                    optimal_num_machines = get<2>(A[i][j][m-1]);
//                }
                for (int k: all_predecessor_ids[j]) {
                    if (i > 0 && std::find(all_predecessor_ids[i-1].begin(), all_predecessor_ids[i-1].end(), k)
                                        != all_predecessor_ids[i-1].end())
                        continue;
                    int max_m_prime = straight_pipeline ? 2 : m+1;
                    for (int m_prime = 1; m_prime < max_m_prime; ++m_prime) {
                        float input_transfer_time = (2.0 * output_activation_sizes[k]) / (bandwidth * m_prime);
                        float output_transfer_time = 0;
                        if (j < output_activation_sizes.size() - 1) {
                            output_transfer_time = (2 * output_activation_sizes[j]) / (bandwidth * m_prime);
                        }
                        float last_stage_time = compute_times[k+1][j];
                        if (last_stage_time == -1){
                            continue;
                        }
                        float last_stage_parameter_size = parameter_sizes[k+1][j];
                        float stashed_data_size = (activation_sizes[k+1][j]) + last_stage_parameter_size;
                        stashed_data_size *= ceil((num_machines - (m+1)) / m_prime);
                        if (stashed_data_size > memory_size){
                            continue;
                        }
                        last_stage_time = last_stage_time + ((4*(m_prime-1)*last_stage_parameter_size)/
                                            (bandwidth * m_prime));
                        last_stage_time /= m_prime;

                        if (get<0>(A[i][k][m-m_prime]) == -1){
                            continue;
                        }
                        float pipeline_time = max(get<0>(A[i][k][m-m_prime]),last_stage_time);

                        // if have activation_compression_ratio

                        if (min_pipeline_time == -1 || min_pipeline_time > pipeline_time) {
                            optimal_split = tuple<int,int>(k, m-m_prime);
                            optimal_num_machines = m_prime;
                            min_pipeline_time = pipeline_time;
                        }

                    }
                }
                A[i][j][m] = tuple<float,pair<int,int>,int>(min_pipeline_time, optimal_split, optimal_num_machines);
            }
        }
    }
    return A;
}

vector<int> analyze_partitioning(vector<vector<vector<tuple<float,pair<int,int>,int>>>> A,
                                 vector<Node> states, int start, int end,
                                 int network_bandwidth, int num_machines, float activation_compression_ratio) {
    tuple<float,pair<int,int>,int> meta_data = A[start][end-1][num_machines-1];
    pair<int,int> next_split = get<1>(meta_data);
    int remaining_machine_left = num_machines;
    vector<int> splits;
    vector<float> replication_factors;
    int prev_split = end - 1;
    while (next_split != make_pair(-1,-1)) {
        int num_machines_used = get<2>(meta_data);
        int tmp1 = next_split.first + 1;
        splits.push_back(next_split.first+1);
        float compute_time = states[prev_split-1].compute_time_ - states[next_split.second].compute_time_;
        float parameter_size = states[prev_split-1].activation_size_ - states[next_split.second].activation_size_;

        float dp_communication_time = (4 * (num_machines_used - 1) * parameter_size ) /
                                      (network_bandwidth * num_machines_used);
        float pp_communication_time_input = (2 * states[next_split.first].output_activation_size_ *
                (1/float(num_machines_used))) / network_bandwidth;
        float pp_communication_time_output = (2 * states[prev_split-1].output_activation_size_ *
                (1/num_machines_used)) / network_bandwidth;
        // if have activation compression ratio
        pp_communication_time_input = 0.0;
        pp_communication_time_output = 0.0;

        compute_time /= num_machines_used;
        dp_communication_time /= num_machines_used;

        prev_split = splits.back();
        meta_data = A[start][next_split.first][next_split.second];
        next_split = get<1>(meta_data);
        replication_factors.push_back(num_machines_used);
        remaining_machine_left -= num_machines_used;
    }
    int num_machines_used = get<2>(meta_data);
    remaining_machine_left -= num_machines_used;
    float compute_time = states[prev_split-1].parameter_size_;
    float parameter_size = states[prev_split-1].parameter_size_;
    float dp_communication_time = ((4*(num_machines_used-1) * parameter_size) /
            (network_bandwidth * num_machines_used));
    compute_time /= num_machines_used;
    dp_communication_time /= num_machines_used;

    prev_split = start;
    splits.reserve(splits.size());
    splits.push_back(end);
    replication_factors.push_back(num_machines_used);
    replication_factors.reserve(replication_factors.size());
    for (int i = 0; i < splits.size(); ++i) {
        float time = 0;
        if (prev_split > 0) {
            time = states[splits[i]-1].compute_time_ - states[prev_split-1].compute_time_;
        } else {
            time = states[splits[i]-1].compute_time_;
        }
        prev_split = splits[i];
    }
    return vector<int>(splits.begin(),splits.end()-1);

}

void optimize_graph(Graph graph, vector<int> all_num_machines, vector<int> network_bandwidths,
                    int memory_size, bool straight_pipeline){

    // 掐头去尾
    vector<Node> sources = graph.sources();
    map<Node,vector<Node>> nodes_to_remove;
    for(Node source: sources) {
        if (startsWith(source.node_desc_,"Input")){
            source.forward_compute_time_ = 0.0;
            source.backward_compute_time_ = 0.0;
            source.activation_size_ = 0.0;
            source.parameter_size_ = 0.0;
            nodes_to_remove[source] = {};
        }
        for (Node out_node: graph.edges_[source.node_id_]){
            nodes_to_remove[source].push_back(out_node);
        }
        graph.remove_node(source);
    }
    vector<Node> sinks = graph.sinks();
    for (Node sink: sinks) {
        if (startsWith(sink.node_desc_,"__getitem__")){
            graph.remove_node(sink);
        }
    }

    Graph antichain_graph = graph.anti_chain_dag();
    vector<Node> states = antichain_graph.topological_sort();

    map<Node,int> states_indices;
    vector<float> output_activation_sizes;
    vector<vector<int>> all_predecessor_ids;
    for (int i = 0; i < states.size(); ++i) {
        states_indices[states[i]] = i;
        // output_activation_sizes.push_back(states[i].output_activation_size_);
        for(string anti_chain_node: states[i].antichain_) {
            states[i].output_activation_size_ += graph.nodes_[anti_chain_node].activation_size_;
        }
    }

    for (int i = 0; i < states.size(); ++i) {
        vector<string> anti_chain = states[i].antichain_;
        set<Node> all_predecessors = graph.all_predecessors(anti_chain);
        states[i].compute_time_ = 0.0;
        states[i].activation_size_ = 0.0;
        states[i].parameter_size_ = 0.0;
        for (Node predecessor: all_predecessors) {
            states[i].compute_time_ += ((predecessor.forward_compute_time_ +
                                        predecessor.backward_compute_time_) / 1000);
            states[i].activation_size_ += predecessor.activation_size_;
            states[i].parameter_size_ += predecessor.parameter_size_;
        }

    }
    graph.reset();

    for (Node state:states) {
        output_activation_sizes.push_back(state.output_activation_size_);
    }
    for (int i = 0; i < states.size(); ++i) {
        for (Node predecessor: antichain_graph.predecessors(states[i].node_id_)){
            all_predecessor_ids.push_back({states_indices[predecessor]});
        }
    }
    vector<vector<float>> compute_times;
    vector<vector<float>> activation_sizes;
    vector<vector<float>> parameter_sizes;
    for (int i = 0; i < states.size() + 1; ++i) {
        vector<float> compute_times_row;
        vector<float> activation_sizes_row;
        vector<float> parameter_sizes_row;
        for (int j = 0; j < states.size(); ++j) {
            if (i == 0) {
                compute_times_row.push_back(states[j].compute_time_);
                activation_sizes_row.push_back(states[j].activation_size_);
                parameter_sizes_row.push_back(states[j].parameter_size_);
            } else {
                if (j > (i-1)) {
                    compute_times_row.push_back(states[j].compute_time_ -
                                                states[i-1].compute_time_);
                    activation_sizes_row.push_back(states[j].activation_size_ -
                                                   states[i-1].activation_size_);
                    parameter_sizes_row.push_back(states[j].parameter_size_ -
                                                    states[i-1].parameter_size_);
                } else {
                    compute_times_row.push_back(-1);
                    activation_sizes_row.push_back(-1);
                    parameter_sizes_row.push_back(-1);
                }
            }
        }
        compute_times.push_back(compute_times_row);
        activation_sizes.push_back(activation_sizes_row);
        parameter_sizes.push_back(parameter_sizes_row);
    }

    int counter = 1;
    int num_machines_in_machine = 1;
    vector<vector<vector<vector<tuple<float,pair<int,int>,int>>>> > all_As;
    for (int i = 0; i < all_num_machines.size(); ++i) {
        int num_machines = all_num_machines[i];
        int network_bandwith = network_bandwidths[i];
        // compute partitioning return A
        vector<vector<vector<tuple<float,pair<int,int>,int>>>> A
            = compute_partitioning(compute_times, activation_sizes,
                                   parameter_sizes, output_activation_sizes,
                                   all_predecessor_ids, num_machines, num_machines_in_machine,
                                   network_bandwith,memory_size, straight_pipeline,
                                   (counter==network_bandwidths.size()));
        num_machines_in_machine = num_machines;
        for (int j = 0; j < compute_times.size(); ++j) {
            for (int k = 0; k < compute_times[0].size(); ++k) {
                // update compute times use partition result
                tuple<float,pair<int,int>,int> tmp = A[j][k].back();
                compute_times[j][k] = get<0>(tmp);
            }
        }
        counter += 1;
        all_As.push_back(A);
    }
    vector<tuple<int, int>> splits;
    splits.push_back(tuple<int,int>(0,states.size()));
    int i = all_As.size() - 1;
    while (i >= 0) {
        vector<tuple<int, int>> new_splits;
        int stage_id = 0;
        for (tuple<int,int> split:splits) {
            // analyze partition
            int start = get<0>(split);
            int end = get<1>(split);
            vector<int> partial_splits =
                    analyze_partitioning(all_As[i],states,start,end,network_bandwidths[i],
                                         all_num_machines[i], -1);
            int start_point = start;
            for(int s:partial_splits) {
                new_splits.push_back(tuple<int,int>(start_point,s));
                if (i == 0){
                    set<Node> predecessors = graph.all_predecessors(states[s-1].antichain_);
                    for (Node predecessor:predecessors) {
                        if (predecessor.stage_id_ == -1) {
                            predecessor.set_stage_id(stage_id);
                        }
                    }
                }
                start_point = s;
                stage_id += 1;
            }
            new_splits.push_back(tuple<int,int>(start_point,end));
            if (i == 0){
                set<Node> predecessors = graph.all_predecessors(states[end-1].antichain_);
                for (Node predecessor:predecessors) {
                    if (predecessor.stage_id_ == -1) {
                        predecessor.set_stage_id(stage_id);
                    }
                }
            }
            stage_id += 1;
        }
        splits = new_splits;
        i -= 1;
    }

    // 还原
    for(auto iter: nodes_to_remove) {
        for (Node out_node:iter.second){
            // iter.first.stage_id_ = 0;
            Node tmp_Node = Node(iter.first);
            tmp_Node.stage_id_ = 0;
            graph.add_edges(tmp_Node, out_node);
        }
    }
    // 生成输出文件

    float total_time = states.back().compute_time_;
    float total_parameter_size = states.back().parameter_size_;
    float data_parallel_total_time = total_time;
    num_machines_in_machine = 1;
    for (int j = 0; j < all_num_machines.size(); ++j) {
        int num_machines = all_num_machines[j];
        int network_bandwidth = network_bandwidths[j];
        float data_parallel_communication_time = (4 * (num_machines - 1) * total_parameter_size)
                        / (network_bandwidth * num_machines) / num_machines_in_machine;
        data_parallel_total_time  = (data_parallel_total_time + data_parallel_communication_time)
                                    / num_machines;
        num_machines_in_machine = num_machines;
    }
//    float pipeline_parallel_total_time = all_As[0][0][states.size()-1][num];
//
    // 输出
}

void graph_optimizer(){

}

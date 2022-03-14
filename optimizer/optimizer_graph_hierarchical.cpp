//
// Created by lenovo on 2022/3/10.
//
#include "optimizer_graph_hierarchical.h"
#include "../utils/util.h"

vector<vector<vector<tuple<float,pair<int,int>,int>>>> compute_partitioning(vector<vector<float>> compute_times,
                                                                            vector<vector<float>> activation_times,
                                                                            vector<vector<float>> parameter_sizes,
                                                                            vector<vector<float>> output_activation_sizes,
                                                                            vector<vector<int>> all_predecessor_ids,
                                                                            int num_machines, int num_machines_within_machine,
                                                                            int bandwidth, int memory_size,
                                                                            bool straight_pipeline, bool final_level) {
    // min_pipeline_time, optimal_splits(start, end), optimal_num_machines
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
            float cum_activation_time = activation_times[i][j];
            float cum_parameter_size = parameter_sizes[i][j];
            int max_m = straight_pipeline ? 1 : num_machines;
            for (int k = 0; k < max_m; ++k) {
                int stashed_data_size = ceil((num_machines - (k+1)) / (k+1))
                                        * (cum_parameter_size + cum_parameter_size);
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
                for (all_predecessor_ids[j]) {

                }
            }
        }
    }
    return A;
}

void optimize_graph(Graph graph, vector<int> all_num_machines, vector<int> network_bandwidths, int memory_size){

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
    vector<int> all_predecessor_ids;
    for (int i = 0; i < states.size(); ++i) {
        states_indices[states[i]] = i;
        output_activation_sizes.push_back(states[i].output_activation_size_);
        for(string anti_chain_node: states[i].antichain_) {
            states[i].output_activation_size_ += graph.nodes_[anti_chain_node].activation_size_;
        }
    }
    for (int i = 0; i < states.size(); ++i) {
        vector<string> anti_chain = states[i].antichain_;
        set<Node> all_predecessors = graph.all_predecessors(anti_chain);
        states[i].compute_time_ = 0.0;
        states[i].activation_size_ = 0.0;
        states[i].parameter_size_;
        for (Node predecessor: all_predecessors) {
            states[i].compute_time_ += ((predecessor.forward_compute_time_ + predecessor.backward_compute_time_) / 1000);
            states[i].activation_size_ += predecessor.activation_size_;
            states[i].parameter_size_ += predecessor.parameter_size_;
        }
        for (Node predecessor: antichain_graph.predecessors(states[i].node_id_)){
            all_predecessor_ids.push_back(states_indices[predecessor]);
        }
    }
    graph.reset();

    vector<vector<float>> compute_times;
    vector<vector<float>> activation_times;
    vector<vector<float>> parameter_sizes;
    for (int i = 0; i < states.size() + 1; ++i) {
        vector<float> compute_times_row;
        vector<float> activation_sizes_row;
        vector<float> parameter_sizes_row;
        for (int j = 0; j < states.size(); ++j) {
            if (i==0) {
                compute_times_row.push_back(states[j].compute_time_);
                activation_sizes_row.push_back(states[j].parameter_size_);
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
        activation_times.push_back(activation_sizes_row);
        parameter_sizes.push_back(parameter_sizes_row);
    }

    int counter = 1;
    int num_machines_in_machine = 1;
    vector<vector<vector<vector<tuple<float,pair<int,int>,int>>>> > all_As;
    for (int i = 0; i < all_num_machines.size(); ++i) {
        int num_machines = all_num_machines[i];
        int network_bandwith = network_bandwidths[i];
        // compute partitioning return A
        vector<vector<vector<tuple<float,pair<int,int>,int>>>> A;
        num_machines_in_machine = num_machines;
        for (int j = 0; j < compute_times.size(); ++j) {
            for (int k = 0; k < compute_times[0].size(); ++k) {
                // update compute times use partition result
                // compute_times[i][j] =
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
        }

    }

}

void graph_optimizer(){

}

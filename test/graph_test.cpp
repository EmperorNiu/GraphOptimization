//
// Created by lenovo on 2022/3/10.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include "../graph/Graph.h"
#include "../utils/util.h"
#include "../optimizer/optimizer_graph_hierarchical.h"

using namespace std;

Graph build_graph(string path){
    ifstream infile("../test/testdata/graph1.txt");
    ostringstream data_stream;
    data_stream << infile.rdbuf();
    string data_str = data_stream.str();
    Graph graph = Graph();
    graph.from_str(data_str);
    infile.close();
    return graph;
}

void test_graph_from_str(){
    ifstream infile("../test/testdata/graph1.txt");
    ostringstream data_stream;
    data_stream << infile.rdbuf();
    string data_str = data_stream.str();
    Graph graph = Graph();
    graph.from_str(data_str);
    infile.close();
} // success

void test_source_and_sink(){
    Graph graph = build_graph("../test/testdata/graph1.txt");
    vector<Node> source_nodes = graph.sources();
    vector<Node> sink_nodes = graph.sinks();
} // success

void test_anti_chain_dag() {
    Graph graph = build_graph("../test/testdata/graph1.txt");
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
}

void test_topological_sort() {
    Graph graph = build_graph("../test/testdata/graph1.txt");
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
    map<Node, int> states_indices;
    vector<float> output_activation_sizes;
    vector<vector<int>> all_predecessor_ids;
    for (int i = 0; i < states.size(); ++i) {
        states_indices[states[i]] = i;
        output_activation_sizes.push_back(states[i].output_activation_size_);
        for(string anti_chain_node: states[i].antichain_) {
            states[i].output_activation_size_ += graph.nodes_[anti_chain_node].activation_size_;
        }
    }
}

void test_states() {
    Graph graph = build_graph("../test/testdata/graph1.txt");
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
    map<Node, int> states_indices;
    vector<float> output_activation_sizes;
    vector<vector<int>> all_predecessor_ids;
    for (int i = 0; i < states.size(); ++i) {
        states_indices[states[i]] = i;
        output_activation_sizes.push_back(states[i].output_activation_size_);
        for(string anti_chain_node: states[i].antichain_) {
            states[i].output_activation_size_ += graph.nodes_[anti_chain_node].activation_size_;
        }
    }
    for (auto & state : states) {
        vector<string> anti_chain = state.antichain_;
        set<Node> all_predecessors = graph.all_predecessors(anti_chain);
        state.compute_time_ = 0.0;
        state.activation_size_ = 0.0;
        state.parameter_size_ = 0.0;
        for (const Node& predecessor: all_predecessors) {
            state.compute_time_ += ((predecessor.forward_compute_time_ +
                                         predecessor.backward_compute_time_) / 1000);
            state.activation_size_ += predecessor.activation_size_;
            state.parameter_size_ += predecessor.parameter_size_;
        }
    }
    for (Node state:states) {
        output_activation_sizes.push_back(state.output_activation_size_);
    }
    for (int i = 0; i < states.size(); ++i) {
        for (Node predecessor: antichain_graph.predecessors(states[i].node_id_)){
            all_predecessor_ids.push_back({states_indices[predecessor]});
        }
    }
}

void test_compute() {
    Graph graph = build_graph("../test/testdata/graph1.txt");
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
    map<Node, int> states_indices;
    vector<float> output_activation_sizes;
    vector<vector<int>> all_predecessor_ids;
    for (int i = 0; i < states.size(); ++i) {
        states_indices[states[i]] = i;
        output_activation_sizes.push_back(states[i].output_activation_size_);
        for(string anti_chain_node: states[i].antichain_) {
            states[i].output_activation_size_ += graph.nodes_[anti_chain_node].activation_size_;
        }
    }
    for (auto & state : states) {
        vector<string> anti_chain = state.antichain_;
        set<Node> all_predecessors = graph.all_predecessors(anti_chain);
        state.compute_time_ = 0.0;
        state.activation_size_ = 0.0;
        state.parameter_size_ = 0.0;
        for (const Node& predecessor: all_predecessors) {
            state.compute_time_ += ((predecessor.forward_compute_time_ +
                                     predecessor.backward_compute_time_) / 1000);
            state.activation_size_ += predecessor.activation_size_;
            state.parameter_size_ += predecessor.parameter_size_;
        }
    }
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
            if (i==0) {
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
}


void test_compute_partitioning() {
    Graph graph = build_graph("../test/testdata/graph1.txt");
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
    map<Node, int> states_indices;
    vector<float> output_activation_sizes;
    vector<vector<int>> all_predecessor_ids;
    for (int i = 0; i < states.size(); ++i) {
        states_indices[states[i]] = i;
        output_activation_sizes.push_back(states[i].output_activation_size_);
        for(string anti_chain_node: states[i].antichain_) {
            states[i].output_activation_size_ += graph.nodes_[anti_chain_node].activation_size_;
        }
    }
    for (auto & state : states) {
        vector<string> anti_chain = state.antichain_;
        set<Node> all_predecessors = graph.all_predecessors(anti_chain);
        state.compute_time_ = 0.0;
        state.activation_size_ = 0.0;
        state.parameter_size_ = 0.0;
        for (const Node& predecessor: all_predecessors) {
            state.compute_time_ += ((predecessor.forward_compute_time_ +
                                     predecessor.backward_compute_time_) / 1000);
            state.activation_size_ += predecessor.activation_size_;
            state.parameter_size_ += predecessor.parameter_size_;
        }
    }
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
            if (i==0) {
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
    vector<int> all_num_machines({4});
    vector<int> network_bandwidths = {1000000000};
    int memory_size = 16000000000;
    bool straight_pipeline = true;

    vector<vector<vector<vector<tuple<float,pair<int,int>,int>>>> > all_As;
    for (int i = 0; i < all_num_machines.size(); ++i) {
        int num_machines = all_num_machines[i];
        int network_bandwith = network_bandwidths[i];
        // compute partitioning return A
        vector<vector<vector<tuple<float,pair<int,int>,int>>>> A
                = compute_partitioning(compute_times, activation_sizes, parameter_sizes, output_activation_sizes,
                                       all_predecessor_ids, num_machines, num_machines_in_machine,
                                       network_bandwith,memory_size, straight_pipeline,
                                       (counter==network_bandwidths.size()));
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
}
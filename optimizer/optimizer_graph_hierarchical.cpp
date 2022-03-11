//
// Created by lenovo on 2022/3/10.
//
#include "optimizer_graph_hierarchical.h"
#include "../utils/util.h"

void optimize_graph(Graph graph, int all_num_machines, int network_bandwidths, int memory_size){

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
    for (int i = 0; i < states.size(); ++i) {
        states_indices[states[i]] = i;
        for(string anti_chain_node: states[i].antichain_) {
            states[i].output_activation_size_ += graph.nodes_[anti_chain_node].activation_size_;
        }
    }
    for (int i = 0; i < states.size(); ++i) {
        vector<string> anti_chain = states[i].antichain_;
        set<Node> all_predecessors = graph.all_predecessors(anti_chain);
    }



}

void graph_optimizer(){

}

//
// Created by lenovo on 2022/3/10.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include "../graph/Graph.h"
#include "../utils/util.h"
using namespace std;

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
    ifstream infile("../test/testdata/graph1.txt");
    ostringstream data_stream;
    data_stream << infile.rdbuf();
    string data_str = data_stream.str();
    infile.close();

    Graph graph = Graph();
    graph.from_str(data_str);
    vector<Node> source_nodes = graph.sources();
    vector<Node> sink_nodes = graph.sinks();
} // success

void test_anti_chain_dag() {
    ifstream infile("../test/testdata/graph1.txt");
    ostringstream data_stream;
    data_stream << infile.rdbuf();
    string data_str = data_stream.str();
    infile.close();

    Graph graph = Graph();
    graph.from_str(data_str);
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

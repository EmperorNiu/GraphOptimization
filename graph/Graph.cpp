//
// Created by lenovo on 2022/3/9.
//

#include "Graph.h"
#include "../utils/util.h"

Graph::Graph() {

}

void Graph::add_node(Node node) {
    nodes_[node.node_id_] = node;
}

void Graph::remove_node(Node node) {

}

void Graph::add_edges(Node node1, Node node2) {

}

void Graph::remove_edges(Node node1, Node node2) {

}

void Graph::from_str(string graph_str) {
    vector<string> graph_str_lines;
    string_split(graph_str, "\n", graph_str_lines);
//    for (string graph_str_line: graph_str_lines) {
//        graph_str_line
//    }
}



//
// Created by lenovo on 2022/3/9.
//

#ifndef PIPEDREAM_GRAPH_H
#define PIPEDREAM_GRAPH_H

#include "Node.h"
#include <vector>
#include <unordered_map>
using namespace std;
class Graph {
public:
//    vector<Node> nodes_;
    unordered_map<string, Node> nodes_; // node id to node
    vector<Node> predecessors_;
    vector<Node> successors_;
    vector<Edge> edges_;
    vector<Edge> in_edges_;

    Graph();
    void add_node(Node node);
    void remove_node(Node node);
    void add_edges(Node node1, Node node2);
    void remove_edges(Node node1, Node node2);

    void from_str(string graph_str);
};




#endif //PIPEDREAM_GRAPH_H

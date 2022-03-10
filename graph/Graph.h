//
// Created by lenovo on 2022/3/9.
//

#ifndef PIPEDREAM_GRAPH_H
#define PIPEDREAM_GRAPH_H

#include "Node.h"
#include <set>
#include <vector>
#include <unordered_map>
using namespace std;
class Graph {
public:
//    vector<Node> nodes_;
    unordered_map<string, Node> nodes_; // node id to node
    unordered_map<string, set<Node>> predecessors_;
    unordered_map<string, set<Node>> successors_;
    unordered_map<string, vector<Node>> edges_;
    unordered_map<string, vector<Node>> in_edges_;

    Graph();

    void add_node(const Node& node);
    void remove_node(const Node& node);
    void add_edges(const Node& node1, const Node& node2);
    void remove_edges(const Node& node1, const Node& node2);

    void from_str(const string& graph_str);

    vector<Node> sources();
    vector<Node> sinks();
    set<Node> predecessors(const string& node_id);
    set<Node> all_predecessors(vector<string> antichain);
    set<Node> successors(const string& node_id);

    void anti_chain_dag();`

};




#endif //PIPEDREAM_GRAPH_H

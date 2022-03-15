//
// Created by lenovo on 2022/3/9.
//

#ifndef PIPEDREAM_GRAPH_H
#define PIPEDREAM_GRAPH_H

#include "Node.h"
#include <set>
#include <map>
#include <vector>
#include <queue>
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
    map<vector<string>, vector<vector<string>>> next_anti_chains_; // sorted string as key
    Graph* anti_chain_graph_ = nullptr;
    map<vector<string>, vector<string>> augmented_anti_chains_;
    map<vector<string>, vector<string>> deaugmented_augmented_anti_chains_;

    Graph() {};

    void add_node(const Node& node);
    void remove_node(const Node& node);
    void add_edges(const Node& node1, const Node& node2);
    void remove_edges(const Node& node1, const Node& node2);
    void reset();

    void from_str(const string& graph_str);

    vector<Node> sources();
    vector<Node> sinks();
    set<Node> predecessors(const string& node_id);
    set<Node> all_predecessors(vector<string> antichain);
    set<Node> successors(const string& node_id);
    vector<Node> topological_sort();
    void topological_sort_helper(const string& node_id, set<string>& marked_nodes, set<string>& tmp_marked_nodes,
                                        queue<string>& sorted_nodes);

    vector<string> construct_anti_chain(vector<string> aug, string old_node, string new_node);
    vector<string> augment_anti_chain(vector<string> anti_chain);
    vector<string> deaugment_augmented_anti_chain(vector<string> aug_anti_chain);
    bool is_next_anti_chain(vector<string> augmented_anti_chain, string new_node_id);
    vector<vector<string>> next_anti_chains(vector<string> anti_chain);
    Graph anti_chain_dag();

    // vector<Graph> partition_graph();
    // Graph partition_graph_helper(int stage_id);
};




#endif //PIPEDREAM_GRAPH_H

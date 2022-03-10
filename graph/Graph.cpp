//
// Created by lenovo on 2022/3/9.
//

#include "Graph.h"
#include <algorithm>
#include "../utils/util.h"

Graph::Graph() {

}

void Graph::add_node(const Node& node) {
    nodes_[node.node_id_] = node;
}

void Graph::remove_node(const Node& node) {
    nodes_.erase(node.node_id_);
    if (edges_.find(node.node_id_) != edges_.end()){
        vector<Node> out_nodes = edges_[node.node_id_];
        edges_.erase(node.node_id_);
        for (Node out_node: out_nodes) {
            vector<Node> out_node_in_edge = in_edges_[out_node.node_id_];
            auto iter = std::remove(out_node_in_edge.begin(),out_node_in_edge.end(),node);
            out_node_in_edge.erase(iter,out_node_in_edge.end());
            in_edges_[node.node_id_] = out_node_in_edge;
        }
    }
    if (in_edges_.find(node.node_id_) != in_edges_.end()){
        vector<Node> in_nodes = in_edges_[node.node_id_];
        in_edges_.erase(node.node_id_);
        for (Node in_node: in_nodes) {
            vector<Node> in_node_in_edge = edges_[in_node.node_id_];
            auto iter = std::remove(in_node_in_edge.begin(),in_node_in_edge.end(),node);
            in_node_in_edge.erase(iter,in_node_in_edge.end());
            edges_[node.node_id_] = in_node_in_edge;
        }
    }
}

void Graph::add_edges(const Node& node1, const Node& node2) {
    if (nodes_.find(node1.node_id_) == nodes_.end()) {
        nodes_[node1.node_id_] = node1;
    }
    if (nodes_.find(node2.node_id_) == nodes_.end()) {
        nodes_[node2.node_id_] = node2;
    }
    if (edges_.find(node1.node_id_) != edges_.end()) {
        edges_[node1.node_id_].push_back(node2);
    }
    edges_[node1.node_id_] = {node2};
    if (in_edges_.find(node2.node_id_) != in_edges_.end()){
        in_edges_[node2.node_id_].push_back(node1);
    }
    in_edges_[node2.node_id_] = {node1};
}

void Graph::remove_edges(const Node& node1, const Node& node2) {
    vector<Node> out_nodes = edges_[node1.node_id_];
    auto iter = std::remove(out_nodes.begin(),out_nodes.end(),node2);
    out_nodes.erase(iter,out_nodes.end());
    edges_[node1.node_id_] = out_nodes;
    vector<Node> in_nodes = in_edges_[node2.node_id_];
    auto iter2 = std::remove(in_nodes.begin(),in_nodes.end(),node1);
    in_nodes.erase(iter,in_nodes.end());
    in_edges_[node2.node_id_] = in_nodes;
}

void Graph::from_str(const string& graph_str) {
    vector<string> graph_str_lines;
    string_split(graph_str, "\n", graph_str_lines);
    for (string graph_str_line: graph_str_lines) {
        if (graph_str_line[0] != '\t'){
            Node node = Node();
            node.from_str(graph_str_line);
        } else {
            vector<string> ids;
            graph_str_line = graph_str_line.substr(1,string::npos);
            string_split(graph_str_line, " -- ", ids);
            if (edges_.find(ids[0]) == edges_.end()) {
                edges_[ids[0]] = {nodes_[ids[1]]};
            } else {
                edges_[ids[0]].push_back(nodes_[ids[1]]);
            }
            if (in_edges_.find(ids[1]) == in_edges_.end()) {
                in_edges_[ids[1]] = {in_edges_[ids[0]]};
            } else {
                in_edges_[ids[1]].push_back(nodes_[ids[0]]);
            }
        }
    }
}

vector<Node> Graph::sources() {
    vector<Node> sources;
    for (auto & node : nodes_) {
        if(in_edges_.find(node.first) == in_edges_.end() || in_edges_[node.first].empty()) {
            sources.push_back(node.second);
        }
    }
    return sources;
}

vector<Node> Graph::sinks() {
    vector<Node> sinks;
    for (auto & node : nodes_) {
        if(edges_.find(node.first) == edges_.end() || edges_[node.first].empty()) {
            sinks.push_back(node.second);
        }
    }
    return sinks;
}

set<Node> Graph::predecessors(const string& node_id) {
    if(predecessors_.find(node_id) != predecessors_.end()){
        return predecessors_[node_id];
    }
    set<Node> predecessors = set<Node>();
    if (in_edges_.find(node_id) == in_edges_.end()){
        return predecessors;
    }
    for (Node in_node:in_edges_[node_id]) {
        predecessors.insert(in_node);
        set<Node> tmp;
        set_union(predecessors.begin(),predecessors.end(),predecessors_[in_node.node_id_].begin(),
                  predecessors_[in_node.node_id_].end(),inserter(tmp,tmp.begin()));
        predecessors = tmp;
    }
    predecessors_[node_id] = predecessors;
    return predecessors;
}

set<Node> Graph::all_predecessors(vector<string> antichain) {
    set<Node> all_predecessors = set<Node>();
    for (string node_id: antichain) {
        all_predecessors.insert(nodes_[node_id]);
        all_predecessors.insert(predecessors_[node_id].begin(),predecessors_[node_id].end());
    }
    return set<Node>();
}

set<Node> Graph::successors(const string &node_id) {
    return set<Node>();
}



Graph Graph::anti_chain_dag() {
    if (anti_chain_graph_!= nullptr) {
        return *anti_chain_graph_;
    }
    Graph anti_dag = Graph();
    int anti_chain_id = 0;
    vector<Node> anti_chain = {this->sources()[0].node_id_};
    AntiChainNode source = AntiChainNode("antichain_0","0");
}

vector<string> Graph::augment_anti_chain(vector<string> anti_chain) {
    sort(anti_chain.begin(),anti_chain.end());
    if (augmented_anti_chains_.find(anti_chain) != augmented_anti_chains_.end()) {
        return augmented_anti_chains_[anti_chain];
    }
    return vector<string>();
}




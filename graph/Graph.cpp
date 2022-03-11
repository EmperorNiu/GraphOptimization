//
// Created by lenovo on 2022/3/9.
//

#include "Graph.h"
#include <algorithm>
#include <map>
#include "../utils/util.h"


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

void Graph::reset() {
    predecessors_ = {};
    successors_ = {};
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
    return all_predecessors;
}

set<Node> Graph::successors(const string &node_id) {
    if (successors_.find(node_id) != successors_.end()) {
        return successors_[node_id];
    }
    set<Node> successors = {};
    if (edges_.find(node_id) == edges_.end()) {
        return successors;
    }
    for (Node out_node: edges_[node_id]) {
        successors.insert(out_node);
        successors.insert(successors_[out_node.node_id_].begin(),successors_[out_node.node_id_].end());
    }
    successors_[node_id] = successors;
    return successors;
}

bool node_compare_by_desc(Node node1, Node node2){
    return node1.node_desc_ < node2.node_desc_;
}

vector<string> Graph::topological_sort() {
    set<string> marked_nodes = {};
    set<string> tmp_marked_nodes = {};
    queue<string> sorted_nodes = {};
    vector<Node> nodes;
    for (auto iter: nodes_) {
        nodes.push_back(iter.second);
    }
    sort(nodes.begin(),nodes.end(), node_compare_by_desc);
    for(Node node: nodes){
        if (marked_nodes.find(node.node_id_) != marked_nodes.end())
            continue;
        topological_sort_helper(node.node_id_,marked_nodes,tmp_marked_nodes,sorted_nodes);
    }
    vector<string> result;
    for (int i=0; i<sorted_nodes.size(); i++) {
        result.push_back(sorted_nodes.front());
        sorted_nodes.pop();
    }
    return result;
}

void Graph::topological_sort_helper(const string& node_id, set<string>& marked_nodes, set<string>& tmp_marked_nodes,
                             queue<string>& sorted_nodes) {
    if (marked_nodes.find(node_id) != marked_nodes.end())
        return;
    if (marked_nodes.find(node_id) != tmp_marked_nodes.end())
        throw "Exist error! Graph has a cycle.";
    tmp_marked_nodes.insert(node_id);
    if (edges_.find(node_id) != edges_.end()) {
        vector<Node> out_nodes = edges_[node_id];
        sort(out_nodes.begin(),out_nodes.end(), node_compare_by_desc);
        for (Node out_node: out_nodes) {
            topological_sort_helper(out_node.node_id_, marked_nodes, tmp_marked_nodes, sorted_nodes);
        }
    }
    marked_nodes.insert(node_id);
    tmp_marked_nodes.insert(node_id);
    sorted_nodes.emplace(node_id);
}

vector<string> Graph::augment_anti_chain(vector<string> anti_chain) {
    sort(anti_chain.begin(),anti_chain.end());
    if (augmented_anti_chains_.find(anti_chain) != augmented_anti_chains_.end()) {
        return augmented_anti_chains_[anti_chain];
    }
    set<string> extra_nodes = set<string>();
    set<Node> all_predecessors = set<Node>();
    for (string anti_chain_node: anti_chain) {
        set<Node> pd = predecessors(anti_chain_node);
        all_predecessors.insert(pd.begin(),pd.end());
        for (const Node& node: pd) {
            for(const Node& out_node: edges_[node.node_id_]) {
                if (pd.find(out_node)==pd.end() && out_node.node_id_ != anti_chain_node) {
                    extra_nodes.insert(node.node_id_);
                }
            }
        }
    }
    for (Node node: extra_nodes) {
        anti_chain.push_back(node.node_id_);
    }
    augmented_anti_chains_[anti_chain] = anti_chain;
    return anti_chain;
}

vector<string> Graph::deaugment_augmented_anti_chain(vector<string> aug_anti_chain) {
    sort(aug_anti_chain.begin(),aug_anti_chain.end());
    if (deaugmented_augmented_anti_chains_.find(aug_anti_chain) != deaugmented_augmented_anti_chains_.end()){
        return deaugmented_augmented_anti_chains_[aug_anti_chain];
    }
    set<string> nodes_to_remove = {};
    for (string augmented_antichain_node: aug_anti_chain) {
        set<Node> successor = successors(augmented_antichain_node);
        for (string augmented_antichain_node_prime: aug_anti_chain) {
            if (successor.find(nodes_[augmented_antichain_node_prime]) != successor.end()){
                nodes_to_remove.insert(augmented_antichain_node);
            }
        }
    }
    vector<string> antichain = {};
    for (string augmented_antichain_node: aug_anti_chain) {
        if (nodes_to_remove.find(augmented_antichain_node) == nodes_to_remove.end()
            && std::find(antichain.begin(), antichain.end(), augmented_antichain_node) == antichain.end()){
            antichain.push_back(augmented_antichain_node);
        }
    }
    deaugmented_augmented_anti_chains_[aug_anti_chain] = antichain;
    return antichain;
}

vector<string> Graph::construct_anti_chain(vector<string> aug, string old_node, string new_node) {
    vector<string> new_anti_chain = {};
    for (string id: aug) {
        if (id != old_node){
            new_anti_chain.push_back(id);
        }
        else {
            new_anti_chain.push_back(new_node);
        }
    }
    vector<string> result;
    return new_anti_chain;
}

vector<vector<string>> Graph::next_anti_chain(vector<string> anti_chain) {
    sort(anti_chain.begin(), anti_chain.end());
    if (next_anti_chains_.find(anti_chain) != next_anti_chains_.end()) {
        return next_anti_chains_[anti_chain];
    }
    vector<vector<string>> next_anti_chains;
    set<string> anti_chain_set(anti_chain.begin(),anti_chain.end());
    vector<string> augmented_anti_chain = augment_anti_chain(anti_chain);
    for (string augmented_anti_chain_node:augmented_anti_chain) {
        vector<Node> next_nodes = {};
        if (edges_.find(augmented_anti_chain_node) != edges_.end()){
            next_nodes = edges_[augmented_anti_chain_node];
        }
        for (Node next_node: next_nodes) {
            if (anti_chain_set.find(next_node.node_id_) != anti_chain_set.end()){
                vector<string> next_anti_chain;
            }
        }
    }

    return next_anti_chains;
}

Graph Graph::anti_chain_dag() {
    if (anti_chain_graph_!= nullptr) {
        return *anti_chain_graph_;
    }
    anti_chain_graph_ = new Graph;
    Graph anti_dag = Graph();
    int anti_chain_id = 0;
    string antichain_node_id;
    vector<string> anti_chain = {this->sources()[0].node_id_};
    AntiChainNode source = AntiChainNode("antichain_0", augment_anti_chain(anti_chain));

    queue<vector<string>> anti_chain_queue;
    map<vector<string>,Node> anti_chain_map;

    while(!anti_chain_queue.empty()){
        vector<string> antichain = anti_chain_queue.back();
        anti_chain_queue.pop();
        sort(antichain.begin(),antichain.end());
        // antichain_key = antichain;
        if (next_anti_chains_.find(antichain) != next_anti_chains_.end())
            continue;
        for (vector<string> next_chain: next_anti_chain(antichain)) {
            sort(next_chain.begin(),next_chain.end());
            if(anti_chain_map.find(next_chain) == anti_chain_map.end()){
                anti_chain_id += 1;
                antichain_node_id = "antichain_0" + to_string(anti_chain_id);
                AntiChainNode next_node = AntiChainNode(antichain_node_id, augment_anti_chain(next_chain));
                anti_chain_map[next_chain] = next_node;
                anti_dag.add_edges(anti_chain_map[antichain],anti_chain_map[next_chain]);
                anti_chain_queue.push(next_chain);
            }
        }

    }
    *anti_chain_graph_ = anti_dag;
    return anti_dag;
}


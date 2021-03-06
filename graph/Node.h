//
// Created by lenovo on 2022/3/9.
//

#ifndef PIPEDREAM_NODE_H
#define PIPEDREAM_NODE_H
#include <string>
#include <vector>

using namespace std;

class Node {
public:
    string node_id_;
    string node_desc_;
    double forward_compute_time_ = 0;
    double backward_compute_time_ = 0;
    double compute_time_ = 0;
    double activation_size_ = 0;
    double parameter_size_ = 0;
    int stage_id_ = -1;
    int depth_ = -1;
    int height_ = -1;

    double output_activation_size_{};
    vector<string> antichain_;
public:
    Node(string node_id, string node_desc, double forward_compute_time, double backward_compute_time,
         double activation_size, double parameter_size, int stage_id, int depth, int height): node_id_(node_id),
         node_desc_(node_desc), forward_compute_time_(forward_compute_time),backward_compute_time_(backward_compute_time),
         activation_size_(activation_size), parameter_size_(parameter_size), stage_id_(stage_id),
         depth_(depth), height_(height),compute_time_(forward_compute_time+backward_compute_time) {};
    Node(string node_id): node_id_(node_id), node_desc_(""),forward_compute_time_(0),backward_compute_time_(0),
         activation_size_(0), parameter_size_(0),stage_id_(-1),compute_time_(0) {};
    Node(string node_id, vector<string> antichain): node_id_(node_id), antichain_(antichain), node_desc_(""),
                                                     forward_compute_time_(0),backward_compute_time_(0),
                                                     activation_size_(0), parameter_size_(0),stage_id_(-1),
                                                     compute_time_(0) {};
    Node() {};
    void set_stage_id(int stage_id){
        stage_id_ = stage_id;
    }
    bool operator==(const Node &node1) const;
    bool operator<(const Node &node1) const;
    bool operator>(const Node &node1) const;
    void from_str(const string& node_str);

};

class Edge {
public:
    string node_id;
    Node* node_ptr;
};

class AntiChainNode: public Node {
public:
    double output_activation_size_;
    vector<string> antichain_;

    AntiChainNode(string node_id, vector<string> antichain) : Node(node_id) {
        antichain_ = antichain;
    }
    AntiChainNode(Node node) {};
};



#endif //PIPEDREAM_NODE_H

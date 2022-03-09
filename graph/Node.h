//
// Created by lenovo on 2022/3/9.
//

#ifndef PIPEDREAM_NODE_H
#define PIPEDREAM_NODE_H
#include <string>

using namespace std;

class Node {
public:
    string node_id_;
    string node_desc_;
    float forward_compute_time_;
    float backward_compute_time_;
    float activation_size_;
    float parameter_size_;
    int stage_id_;
    int depth_;
    int height_;
public:
    Node(string node_id, string node_desc, float forward_compute_time, float backward_compute_time,
         float activation_size, float parameter_size, int stage_id, int depth, int height): node_id_(node_id),
         node_desc_(node_desc), forward_compute_time_(forward_compute_time),backward_compute_time_(backward_compute_time),
         activation_size_(activation_size), parameter_size_(parameter_size), stage_id_(stage_id),
         depth_(depth), height_(height) {};
    Node(string node_id): node_id_(node_id), node_desc_(""),forward_compute_time_(0),backward_compute_time_(0),
         activation_size_(0), parameter_size_(0) {};

    void set_stage_id(int stage_id){
        stage_id_ = stage_id;
    }

    void from_str(string node_str);

};

class Edge {
public:
    string node_id;
    Node* node_ptr;
};

class AntichainNode(Node) {

}



#endif //PIPEDREAM_NODE_H
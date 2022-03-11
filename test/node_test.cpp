//
// Created by lenovo on 2022/3/9.
//
#include <iostream>
#include "../graph/Node.h"
#include "../graph/Graph.h"
#include<string>

void test_node_from_str(){
    string str = "node9 -- Conv2d(128, 128, kernel_size=(3, 3), stride=(1, 1), padding=(1, 1)) -- forward_compute_time=3.592, "
                 "backward_compute_time=53.858, activation_size=822083584.000, parameter_size=590336.000 -- stage_id=0";
    Node node = Node();
    node.from_str(str);
    cout << node.node_id_ << endl;
    cout << node.parameter_size_ << endl;
    cout << node.node_desc_ << endl;
}
//using namespace std;
//int main(){
//  std::string id = "node1";
//  Node node1 = Node(id);
//  string str = "node9 -- Conv2d(128, 128, kernel_size=(3, 3), stride=(1, 1), padding=(1, 1)) -- forward_compute_time=3.592, backward_compute_time=53.858, activation_size=822083584.000, parameter_size=590336.000 -- stage_id=0";
//  Node node = Node();
//  node.from_str(str);
//  cout << node.node_id_ << endl;
//  cout << node.parameter_size_ << endl;
//  cout << node.node_desc_ << endl;
//}


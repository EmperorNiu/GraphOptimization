//
// Created by lenovo on 2022/3/9.
//

#include "Node.h"
#include "../utils/util.h"
#include <vector>

using namespace std;

void Node::from_str(string node_str) {
    vector<string> node_str_tokens;
    string_split(node_str, " -- ", node_str_tokens);
    node_id_ = node_str_tokens[0];
    node_desc_ = node_str_tokens[1];
    string node_metadata = node_str_tokens[2];
    if (node_str_tokens.size() > 3){
        vector<string> stage_tokens;
        string_split(node_str_tokens[3], "=", stage_tokens);
        stage_id_ = stoi(stage_tokens[1]);
    }
    vector<string> node_metadata_tokens;
    string_split(node_metadata, ", ", node_metadata_tokens);
    forward_compute_time_ = attribute_split(node_metadata_tokens[0]);
    backward_compute_time_ = attribute_split(node_metadata_tokens[1]);
    parameter_size_ = attribute_split(node_metadata_tokens[3]);
    string activation = node_metadata_tokens[2];
    string param = node_str_tokens[3];
    if (activation.find('[') != string::npos){
        vector<string> str_tokens;
        string_split(activation, "=", str_tokens);
        activation = str_tokens[1];
        string_split(activation, "; ", str_tokens);
        float sum = 0.0;
        for (string token: str_tokens) {
            sum += stof(token);
        }
        activation_size_ = sum;
    } else {
        activation_size_ = attribute_split(activation);
    }
}

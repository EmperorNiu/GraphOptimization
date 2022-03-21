//
// Created by lenovo on 2022/3/10.
//

#ifndef PIPEDREAM_OPTIMIZER_GRAPH_HIERARCHICAL_H
#define PIPEDREAM_OPTIMIZER_GRAPH_HIERARCHICAL_H

#include <map>
#include <vector>
#include "../graph/Node.h"
#include "../graph/Graph.h"

using namespace std;

vector<vector<vector<tuple<double,pair<int,int>,int>>>> compute_partitioning(vector<vector<double>> compute_times,
                                                                            vector<vector<double>> activation_sizes,
                                                                            vector<vector<double>> parameter_sizes,
                                                                            vector<double> output_activation_sizes,
                                                                            vector<vector<int>> all_predecessor_ids,
                                                                            int num_machines, int num_machines_within_machine,
                                                                            int bandwidth, long memory_size,
                                                                            bool straight_pipeline, bool final_level);
vector<int> analyze_partitioning(vector<vector<vector<tuple<double,pair<int,int>,int>>>> A,
                                 vector<Node> states, int start, int end,
                                 int network_bandwidth, int num_machines, double activation_compression_ratio);
void optimize_graph(Graph graph, vector<int> all_num_machines, vector<int> network_bandwidths, int memory_size);

#endif //PIPEDREAM_OPTIMIZER_GRAPH_HIERARCHICAL_H

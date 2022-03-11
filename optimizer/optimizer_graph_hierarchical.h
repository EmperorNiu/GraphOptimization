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

void compute_partitioning();
void analyze_partitioning();
void optimize_graph(Graph graph, int all_num_machines, int network_bandwidths, int memory_size);

#endif //PIPEDREAM_OPTIMIZER_GRAPH_HIERARCHICAL_H

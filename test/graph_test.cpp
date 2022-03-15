//
// Created by lenovo on 2022/3/10.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include "../graph/Graph.h"

using namespace std;

const streamsize MAX = 80;
void test_graph_from_str(){
    ifstream infile("../test/testdata/graph1.txt");
    ostringstream data_stream;
    data_stream << infile.rdbuf();
    string data_str = data_stream.str();
    Graph graph = Graph();
    graph.from_str(data_str);
//    if (infile.is_open()) {
//        cout << "read data from graph1:\n";
//        while (infile >> data_tmp) {
//            data += data_tmp;
//        }
//    }
//    cout << data_str;
    infile.close();
}
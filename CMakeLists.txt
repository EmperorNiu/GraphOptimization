cmake_minimum_required(VERSION 3.20)
project(pipedream)

set(CMAKE_CXX_STANDARD 14)

add_executable(pipedream main.cpp graph/Node.cpp graph/Node.h graph/Graph.cpp graph/Graph.h utils/util.cpp utils/util.h test/node_test.cpp optimizer/optimizer_graph_hierarchical.cpp test/graph_test.cpp)

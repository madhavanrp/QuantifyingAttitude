//
// Created by Madhavan Rajagopal Padmanabhan on 2019-08-14.
//

#include "RISCalcSpaceEfficient.hpp"

RISCalcSpaceEfficient::RISCalcSpaceEfficient(Graph *graph) {
    sfmt_init_gen_rand(&sfmt, rand());
    //Default Epsilon
    this->graph = graph;
    this->epsilon = 2;
}

RISCalcSpaceEfficient::RISCalcSpaceEfficient(Graph *graph, double epsilon) {
    sfmt_init_gen_rand(&sfmt, rand());
    this->graph = graph;
    this->epsilon = epsilon;
}

void RISCalcSpaceEfficient::initDataStructures() {
    int m = graph->getNumberOfEdges();
    int n = graph->getNumberOfVertices();

    this->visited = vector<bool>(n,false);
    this->visitMark = vector<int>(n,0);
    this->seed = vector<bool>(n,false);

    this->R = calculateNumberOfRRSets();
}

double RISCalcSpaceEfficient::calculateNumberOfRRSets() {
    return 0;
}
double RISCalcSpaceEfficient::estimate(std::set<int> seedSet) {
    return 0;
}
//
// Created by Madhavan Rajagopal Padmanabhan on 2019-08-14.
//

#ifndef INFLUENCEMAXIMIZATION_RISCALCSPACEEFFICIENT_HPP
#define INFLUENCEMAXIMIZATION_RISCALCSPACEEFFICIENT_HPP

#include <InfluenceMaximization/Graph/Graph.hpp>
#include <set>
#include <cmath>

class RISCalcSpaceEfficient {
protected:
    Graph *graph;
    double epsilon;
    sfmt_t sfmt;
    vector<bool> seed;
    deque<int> q;
    double R;


    vector<bool> visited;
    vector<int> visitMark;
public:
    RISCalcSpaceEfficient(Graph *graph);
    RISCalcSpaceEfficient(Graph *graph, double epsilon);
    void initDataStructures();
    virtual double calculateNumberOfRRSets();
    virtual double estimate(std::set<int> seedSet);
};


#endif //INFLUENCEMAXIMIZATION_RISCALCSPACEEFFICIENT_HPP

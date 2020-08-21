//
// Created by Madhavan Rajagopal Padmanabhan on 2019-07-23.
//

#ifndef INFLUENCEMAXIMIZATION_ATTITUDEMAXIMIZER_HPP
#define INFLUENCEMAXIMIZATION_ATTITUDEMAXIMIZER_HPP

#include <set>
#include <memory>
#include <InfluenceMaximization/Graph/Graph.hpp>
#include <math.h>
#include "InfluenceMaximization/TIM-Models/TIMCoverage.hpp"

class AttitudeMaximizer {
public:
    double epsilon1, epsilon2, epsilon3;
    Graph *graph;
    double delta;
    sfmt_t sfmt;
    vector<bool> visited;
    vector<int> visitMark;
    deque<int> q;
    vector<vector<int>> rrSets;

    shared_ptr<TIMCoverage> rasCoverage;
    shared_ptr<vector<vector<int>>> lookupTable;
public:
    AttitudeMaximizer(Graph *graph);
    set<int> findMaximizingSeed(int k);
    void generateNewRRSets(int R);
    void generateRandomRRSet(int randomVertex, vector<vector<int>> *rrSets);
    double estimateAttitude(std::set<int> candidateSet, double delta, int maxRRSets);
    int getNumberOfRRSetsGenerated();
};


#endif //INFLUENCEMAXIMIZATION_ATTITUDEMAXIMIZER_HPP

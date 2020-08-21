//
// Created by Madhavan R.P on 2019-06-11.
//

#ifndef INFLUENCEMAXIMIZATION_ATTITUDECALCULATOR_HPP
#define INFLUENCEMAXIMIZATION_ATTITUDECALCULATOR_HPP

#include <stdio.h>
#include "InfluenceMaximization/TIM-Models/TIMCoverage.hpp"
#include "SFMT/SFMT.h"
#include "Graph.hpp"
#include <set>
#include <cmath>
#include <memory>

class AttitudeCalculator{
    Graph *graph;
    double epsilon;
    vector<vector<int>> rrSetsTargets;
    sfmt_t sfmt;

    shared_ptr<TIMCoverage> timCoverageTargets;
    shared_ptr<vector<vector<int>>> lookupTableTargets;
    deque<int> q;


    vector<bool> visited;
    vector<int> visitMark;
    vector<int> targetCounts;

public:
    AttitudeCalculator(Graph *graph);
    AttitudeCalculator(Graph *graph, double epsilon);
    void constructCalculator(Graph *graph, double epsilon);
    void generateRandomRRSets(int R);
    void generateRandomRRSet(int randomVertex, vector<vector<int>> *rrSets, vector<int> *counts);
    double getScalingFactor();
    int findAttitude(set<int> seedSet);
    vector<vector<int>> *getRRsets();
    shared_ptr<TIMCoverage> getTIMCoverage();
};


#endif //INFLUENCEMAXIMIZATION_ATTITUDECALCULATOR_HPP

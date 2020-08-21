//
// Created by Madhavan Rajagopal Padmanabhan on 2020-01-13.
//

#ifndef INFLUENCEMAXIMIZATION_ATTITUDEMINUSINFMAXIMIZER_HPP
#define INFLUENCEMAXIMIZATION_ATTITUDEMINUSINFMAXIMIZER_HPP

#include <unordered_map>
#include <vector>
#include <stack>
#include <assert.h>
#include <InfluenceMaximization/Graph/Graph.hpp>
#include <InfluenceMaximization/TIM-Models/RISRRGraph.hpp>
#include <InfluenceMaximization/Attitude/AttitudeMaximizer.hpp>

using namespace std;

// An adjacency list data structure. Key, Value - Vertex: [Count, [List of edges]]

class AttitudeMinusInfMaximizer {

protected:
    Graph *graph;
    double delta;
    sfmt_t sfmt;
    vector<bool> visited;
    vector<int> visitMark;
    deque<int> q;
    vector<vector<int>> rrSets;

    shared_ptr<vector<vector<int>>> lookupTable;
    vector<RandomGraph> randomGraphs;
    vector<int> startVertices;
    shared_ptr<RISRRGraph> rasCoverage;
    set<int> seedSet;
    vector<int> orderedSeed;
public:
//    using AttitudeMaximizer::AttitudeMaximizer;
    AttitudeMinusInfMaximizer(Graph *graph);
    void generateRandomRRSet(int randomVertex, vector<vector<int>> *rrSets);
    void generateInitialSets();
    set<int> findMaximizingSeed(int k);
    vector<int> getOrderedSeed();
};


#endif //INFLUENCEMAXIMIZATION_ATTITUDEMINUSINFMAXIMIZER_HPP

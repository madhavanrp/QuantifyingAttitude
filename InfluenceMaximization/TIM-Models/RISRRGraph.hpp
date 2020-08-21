//
// Created by Madhavan Rajagopal Padmanabhan on 2020-01-14.
//

#ifndef INFLUENCEMAXIMIZATION_RISRRGRAPH_HPP
#define INFLUENCEMAXIMIZATION_RISRRGRAPH_HPP

#include "InfluenceMaximization/TIM-Models/TIMCoverage.hpp"
#include <stack>

// Random graph structure: vertex-> [count, <to1, to2, to3>]
//typedef unordered_map<int,pair<int,std::vector<int>>> RandomGraph;
class RandomGraph{
public:
    unordered_set<int> vertices;
    unordered_map<int, int> counts;
    unordered_map<int, vector<int>> edges;

};


class RISRRGraph: public TIMCoverage {
public:
    using TIMCoverage::TIMCoverage;
    set<int> seedSet;
    vector<int> orderedSeed;

    vector<int> counts;

    // The random graphs and the vertices from which they were generated
    vector<RandomGraph> *randomGraphs;
    vector<int> startVertices;

    // Initializing the datastructures needed.
    void initializeFromRRGraphs(vector<RandomGraph> *randomGraphs, vector<int> startVertices, int n);
    void initializeDataStructures(int R, int n);
    void calculateRandomGraphCounts();
    void addToSeed(int vertex);
    pair<int, double> findMaxInfluentialNode();
    void removeVertexAndRecalculate(RandomGraph &randomGraph, int randomGraphID, int vertexToRemove);
    void performBFS(RandomGraph &g, int startVertex, int vertexRemoved);
    set<int> findTopKNodes(int k);
    vector<int> getOrderedSeed();

};


#endif //INFLUENCEMAXIMIZATION_RISRRGRAPH_HPP

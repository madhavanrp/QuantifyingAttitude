//
// Created by Madhavan Rajagopal Padmanabhan on 2020-01-04.
//

#ifndef INFLUENCEMAXIMIZATION_DIFFUSIONBASE_HPP
#define INFLUENCEMAXIMIZATION_DIFFUSIONBASE_HPP

#include <vector>
#include "InfluenceMaximization/Graph/Graph.hpp"
#include <set>

using namespace std;

//struct QueueComparatorForDiffusion {
//    bool operator()(pair<int, double> a, pair<int, double> b)
//    {
//        return a.second < b.second;
//    }
//};

class DiffusionBase {
protected:
    double outputValue;
    static const int NUMBER_OF_SIMULATIONS = 10000;
    Graph* myGraph;
    int n; //number of nodes in graph
    set<int> seedSet;
    double total;
public:
    DiffusionBase(Graph* graph);
    virtual double performOperation()=0;
    double performDiffusion(set<int> seedSet); //this is different from the diffusion for influence

};


#endif //INFLUENCEMAXIMIZATION_DIFFUSIONBASE_HPP

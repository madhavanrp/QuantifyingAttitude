//
// Created by Xiaoyu Fu on 6/10/2019.
//

/* This class performs a diffusion under Belief-IC model for a predefined constant number of times.
 * In each diffusion, calculate and store the belief of each node.
 * Use a matrix to store the belief value of each node in each of these diffusions.
 * Then use a vector to store the averaged belief for each node.
 *
 * A diffusion starts with a given seed set, and then expand in a probability-based BFS manner until
 * no new nodes are activated. The belief of a node in a single diffusion is the number of times it gets "activated".
 */
#ifndef BELIEFMAXIMIZATION_DIFFUSION_H
#define BELIEFMAXIMIZATION_DIFFUSION_H

#include <vector>
#include "InfluenceMaximization/Graph/Graph.hpp"
#include <set>

using namespace std;

struct QueueComparatorForDiffusion {
    bool operator()(pair<int, double> a, pair<int, double> b)
    {
        return a.second < b.second;
    }
};

class Diffusion {
private:
    vector<vector<double>> beliefMatrix; //stores the belief of each node in each round
    vector<double > averageBelief; //stores average belief of each node
    double totalBelief; //stores belief of the network: sum of the average beliefs of each node
    static const int NUMBER_OF_SIMULATIONS = 10000;
    Graph* myGraph;
    int n; //number of nodes in graph
//    int m;


public:
    Diffusion(Graph* graph);
    void performDiffusion(set<int> seedSet); //this is different from the diffusion for influence
    void singleDiffusion(set<int> seedSet, int roundNum);//computes the belief for each node in this diffusion


    vector<double>* getBeliefVector(); //this should be called after performDiffusion()
    double getBeliefOfNode(int node);//this should be called after performDiffusion()
    double getTotalBelief();//this should be called after performDiffusion()
    vector<pair<int, double>> findTopXBeliefNodes(int X); //this should be called after performDiffusion()

};


#endif //BELIEFMAXIMIZATION_DIFFUSION_H

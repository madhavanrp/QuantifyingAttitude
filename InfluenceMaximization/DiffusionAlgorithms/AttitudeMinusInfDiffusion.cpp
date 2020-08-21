//
// Created by Madhavan Rajagopal Padmanabhan on 2020-01-04.
//

#include "AttitudeMinusInfDiffusion.hpp"

double AttitudeMinusInfDiffusion::performOperation() {
    queue<int> myQueue;
    vector<bool> visited(this->n, false);
    for(int seedNode : seedSet){
        myQueue.push(seedNode);
        visited[seedNode] = true;
    }
    vector<vector<int>>* graphMatrix = this->myGraph->getGraph();
    int attitude = (int)seedSet.size();
    int influence = 0;
    while (!myQueue.empty())
    {
        int u = myQueue.front();
        influence++;
        myQueue.pop();
        vector<int> neighbors = (*graphMatrix)[u];
        for(int v : neighbors) //edge u->v in graph
        {
            if (myGraph->flipCoinOnEdge(u, v))
            {
                attitude++;
                if (!visited[v]) {
                    visited[v] = true;
                    myQueue.push(v);
                }
            }
        }
    }
    attitudeEstimation+=attitude;
    influenceEstimation+=influence;
    ratioEstimation+= (double)attitude/(double)influence;
    attMinusInfluenceEstimation+= (attitude-influence);
    return (attitude-influence);
}

double AttitudeMinusInfDiffusion::performDiffusion(set<int> seedSet) //this is different from the diffusion for influence
{
    this->total = 0;
    this->seedSet = seedSet;
    for(int i = 0; i < NUMBER_OF_SIMULATIONS; i++)
    {
        total+=performOperation();
    }
    double average = (total)/(double)NUMBER_OF_SIMULATIONS;
    attitudeEstimation = (attitudeEstimation)/(double)NUMBER_OF_SIMULATIONS;
    influenceEstimation = (influenceEstimation)/(double)NUMBER_OF_SIMULATIONS;
    ratioEstimation = (ratioEstimation)/(double)NUMBER_OF_SIMULATIONS;
    attMinusInfluenceEstimation = (attMinusInfluenceEstimation)/(double)NUMBER_OF_SIMULATIONS;
    cout << "\n Attitude estimation: " << attitudeEstimation;
    cout << "\n Influence estimation: " << influenceEstimation;
    cout << "\n Ratio estimation: " << ratioEstimation;
    cout << "\n AttMinusInfluence Estimation: " << attMinusInfluenceEstimation;
    return average;
}

double AttitudeMinusInfDiffusion::getAttitudeEstimation() {
    return attitudeEstimation;
}

double AttitudeMinusInfDiffusion::getInfluenceEstimation() {
    return influenceEstimation;
}

double AttitudeMinusInfDiffusion::getRatioEstimation() {
    return ratioEstimation;
}

double AttitudeMinusInfDiffusion::getAttMinusInfluenceEstimation() {
    return attMinusInfluenceEstimation;
}

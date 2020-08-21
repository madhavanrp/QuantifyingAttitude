//
// Created by Madhavan Rajagopal Padmanabhan on 2020-01-04.
//

#include "DiffusionBase.hpp"
DiffusionBase::DiffusionBase(Graph* graph)
{
    myGraph = graph;
    n = myGraph->getNumberOfVertices();

}

double DiffusionBase::performDiffusion(set<int> seedSet) //this is different from the diffusion for influence
{
    this->total = 0;
    this->seedSet = seedSet;
    for(int i = 0; i < NUMBER_OF_SIMULATIONS; i++)
    {
        total+=performOperation();
    }
    double average = (total)/(double)NUMBER_OF_SIMULATIONS;
    return average;
}
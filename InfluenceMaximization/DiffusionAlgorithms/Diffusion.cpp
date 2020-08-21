//
// Created by Xiaoyun Fu on 6/10/2019.
//

#include "Diffusion.h"
#include <queue>


Diffusion::Diffusion(Graph* graph)
{
    myGraph = graph;
    n = myGraph->getNumberOfVertices();
//    int m = this->graph->getNumberOfEdges();
    //check for bugs!! initialize 2d vector
    //vector<vector<int>> matrix(M, vector<int>(N, default_value));
//    beliefMatrix = vector<vector<double>>(NUMBER_OF_SIMULATIONS, vector<double>(n, 0.0));
//    for(int i = 0; i < NUMBER_OF_SIMULATIONS; i++) {
//        beliefMatrix.push_back(vector<double>(n, 0.0));
//    }
//    beliefMatrix = vector<double>(NUMBER_OF_SIMULATIONS, vector<double>(n, 0.0));; //stores the belief of each node in each round
                                        //each row stores a vector for a single diffusion
    averageBelief = vector<double>(n, 0.0); //stores average belief of each node
    totalBelief = 0.0; //stores belief of the network: sum of the average beliefs of each node

}

void Diffusion::performDiffusion(set<int> seedSet) //this is different from the diffusion for influence
{
   for(int i = 0; i < NUMBER_OF_SIMULATIONS; i++) {
       singleDiffusion(seedSet, 0);
       if (i % 2000 == 0) {
           cout << "\n Finished diffusions: " << i << flush;
       }
   }

//       cout << "the " << i << "th diffusion performed" << endl;
   //compute averageBelief
//   for(int i = 0; i < averageBelief.size(); i++)
//   {  //i is the node
//       double sum = 0;
//       for(int j = 0; j < beliefMatrix.size(); j++) //j is the diffusion round index
//       {
//           sum += beliefMatrix[j][i];
//       }
//       averageBelief[i] = sum / beliefMatrix.size();
//   }

   //compute totalBelief
   for(int i = 0; i < averageBelief.size(); i++) {
       averageBelief[i] = averageBelief[i]/ (double)NUMBER_OF_SIMULATIONS;
       totalBelief += averageBelief[i];
   }
}

void Diffusion::singleDiffusion(set<int> seedSet, int roundNum)//computes the belief for each node in this diffusion
{
    queue<int>* myQueue = new queue<int>();
    vector<bool> visited(n, false);
    for(int seedNode : seedSet){
        myQueue->push(seedNode);
        visited[seedNode] = true;
    }
    vector<vector<int>>* graphMatrix = myGraph->getGraph();
    while (!myQueue->empty())
    {
        int u = myQueue->front();
        myQueue->pop();
        vector<int> neighbors = (*graphMatrix)[u];
        for(int v : neighbors) //edge u->v in graph
        {
            if (myGraph->flipCoinOnEdge(u, v))
            {
//                cout << "node " << v << "'s belief is increased by 1" << endl;
                if (!visited[v])
                { //v is visited means v is activated. Once a node v is activated, it only has one chance of activating its neighbors
                   visited[v] = true;
                   myQueue->push(v);  //thus we only add v to queue once
                }
                // v's belief is increased by 1
                averageBelief[v]+=1;
            }
        }
    }
    delete myQueue;
}



vector<double>* Diffusion::getBeliefVector()
{
    return &averageBelief;
}

double Diffusion::getBeliefOfNode(int node)
{
    if (node >= n || node < 0)
        throw node;
    return averageBelief[node];
}

double Diffusion::getTotalBelief()
{
    return totalBelief;
}

//returns the X nodes with highest average belief, result[0] is the node with highest belief
vector<pair<int, double>> Diffusion::findTopXBeliefNodes(int X) //this should be called after performDiffusion()
{
   vector<int> result(X, -1);  //-1 is not a valid node, nodes are 0 through n-1
    vector<pair<int, double>> allPairs;
    for(int i = 0; i < averageBelief.size(); i++)
    {
        pair<int, double> pair = make_pair(i,averageBelief[i] );
        allPairs.push_back(pair);
    }
   priority_queue<pair<int, double>, vector<pair<int, double>>, QueueComparatorForDiffusion> queue;
   //initialize queue with <node,beliefOfNode>
   for(int i = 0; i < allPairs.size(); i++)
   {
       pair<int, double> pair = allPairs[i];
       queue.push(pair);
   }

//find top K largest element with beliefs
    vector<pair<int, double>> highAttitude;
    for(int i = 0;i < X; i++)
    {
        pair<int,double> element = queue.top();
        queue.pop();
        result[i] = element.first;
        highAttitude.push_back(element);
    }


//   vector<double> tempBelief(averageBelief.size(), -1.0);
//   for(int i = 0; i < averageBelief.size(); i++)
//   {
//       tempBelief[i] = averageBelief[i];
//   }
//   for(int i = 0; i < X; i++)
//   {
//       int maxBeliefIndex = 0;
//       for(int j = 1; j < tempBelief.size(); j++ )
//       {
//           cout << "belief of node " << j << " = " << tempBelief[j] << endl;
//           if (tempBelief[j] > tempBelief[maxBeliefIndex])
//               maxBeliefIndex = j;
//       }
//       cout << i << "th largest belief node " << maxBeliefIndex << ", Belief = " << tempBelief[maxBeliefIndex] << endl;
//       result[i] = maxBeliefIndex;
//       tempBelief[maxBeliefIndex] = -1;
//   }

    return highAttitude;

}
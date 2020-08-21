//
// Created by Madhavan Rajagopal Padmanabhan on 2019-08-08.
//

#include "AttitudeCalcSpaceEfficient.hpp"

double AttitudeCalcSpaceEfficient::calculateNumberOfRRSets() {
    int m = graph->getNumberOfEdges();
    cout << "\n Old Number of RR Sets " << (8+2 * epsilon) * m * (2 * log(m) + log(2))/(epsilon * epsilon);
    epsilon = 0.1;
//    double delta = (double)1/(double)graph->getNumberOfVertices();
    double delta = 0.001;
    double newRRSets = (2+ epsilon) * m * (log(2) - log(delta))/(epsilon * epsilon);
    newRRSets = ceil(newRRSets);
    cout << "\n New number of RRSets " << newRRSets;
    cout << "\n n = " << graph->getNumberOfVertices() << " m = " << m;
    cout << "\n Delta " << delta << flush;

    return newRRSets;
}

double AttitudeCalcSpaceEfficient::estimate(std::set<int> seedSet) {
    for (int v:seedSet) {
        this->seed[v] = true;
    }

    int n = this->graph->getNumberOfVertices();
    int m = this->graph->getNumberOfEdges();
    vector<pair<int,int>> *edgeSet = this->graph->getEdgeSet();
    pair<int,int> randomEdge;
    int randomEdgeID, randomVertex;
    bool found;
    int total = 0;
    int reset = 0;
    // DO RAS
    for (double i = 0; i < this->R; ++i) {

//        if (reset%5000000==0) {
//            cout << "\n Finished iterations " << i;
//            cout << "\n To Go:  " << this->R - i << flush;
//            reset = 0;
//        }
        reset++;
        randomEdgeID = sfmt_genrand_uint32(&sfmt) % m;
        randomEdge = (*edgeSet)[randomEdgeID];
        if(!graph->flipCoinOnEdge(randomEdge.first, randomEdge.second)) continue;
        if(seed[randomEdge.first]) {
            total++;
            continue;
        }
        q.clear();
        randomVertex = randomEdge.first;
        q.push_back(randomVertex);
        int nVisitMark = 0;
        visitMark[nVisitMark++] = randomVertex;
        visited[randomVertex] = true;
        vector<vector<int>> *graphTranspose = graph->getGraphTranspose();
        found = false;
        while(!q.empty() && !found) {
            int u=q.front();
            q.pop_front();
            for(int j=0; j<(int)(*graphTranspose)[u].size(); j++){
                int v = (*graphTranspose)[u][j];
                if(!graph->flipCoinOnEdge(v, u))
                    continue;
                if(visited[v])
                    continue;

                visitMark[nVisitMark++]=v;
                visited[v]=true;
                q.push_back(v);
                if (seed[v]) {
                    found = true;
                    total++;
                    break;
                }
            }
        }
        for(int j=0;j<nVisitMark;j++) {
            visited[visitMark[j]] = false;
        }
    }


    for (int v:seedSet) {
        this->seed[v] = false;
    }

    return (double) total * (double) m/R;
}
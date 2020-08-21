//
// Created by Madhavan Rajagopal Padmanabhan on 2019-08-14.
//

#include "InfluenceCalcSpaceEfficient.hpp"
double InfluenceCalcSpaceEfficient::calculateNumberOfRRSets() {
    int n = this->graph->getNumberOfVertices();
    return (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
}

double InfluenceCalcSpaceEfficient::estimate(std::set<int> seedSet) {
    for (int v:seedSet) {
        this->seed[v] = true;
    }

    int n = this->graph->getNumberOfVertices();
    int randomVertex;
    bool found;
    int total = 0;
    // DO RAS
    for (double i = 0; i < this->R; ++i) {

        q.clear();
        randomVertex = sfmt_genrand_uint32(&sfmt) % n;
        if (seed[randomVertex]) {
            total++;
            continue;
        }
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
        for(int i=0;i<nVisitMark;i++) {
            visited[visitMark[i]] = false;
        }
    }


    for (int v:seedSet) {
        this->seed[v] = false;
    }

    return (double) total * (double) n/R;
}
//
// Created by Madhavan Rajagopal Padmanabhan on 2019-07-23.
//

#include "AttitudeMaximizer.hpp"

AttitudeMaximizer::AttitudeMaximizer(Graph *graph) {
    sfmt_init_gen_rand(&sfmt, rand());
    this->graph = graph;
    for(int i=0;i<graph->getNumberOfVertices();i++) {
        this->visitMark.push_back(0);
        this->visited.push_back(false);
    }

    //RAS Data Structure
    this->lookupTable.reset(new vector<vector<int>>());
    this->rasCoverage.reset(new TIMCoverage(this->lookupTable.get()));

}

set<int> AttitudeMaximizer::findMaximizingSeed(int k) {
    double epsilon = 0.1;
    this->epsilon1 = epsilon/(double)6;
    this->epsilon2 = epsilon/(double)2;
    this->epsilon3 = epsilon/(double)(4*(1-1/exp(1)));
    double delta = 1/(double)graph->getNumberOfVertices();
    double lambda = (4* exp(1) - 8 ) * (1+ epsilon1) * (1+epsilon2) * (log(3/delta))/pow(this->epsilon3,2);

//    cout << "\n Epsilon 1 : " << this->epsilon1;
//    cout << "\n Epsilon 2 : " << this->epsilon2;
//    cout << "\n Epsilon 3 : " << this->epsilon3;
//    cout << "\n Delta: " << delta;
//    cout << "\n Lambda: " << lambda;
//    cout << flush;
    int m = this->graph->getNumberOfEdges();
    int n = this->graph->getNumberOfVertices();

    // Generate initial set of rr sets
    generateNewRRSets(lambda);
    this->rasCoverage->initializeLookupTable(&rrSets, n);
    this->rasCoverage->initializeDataStructures(lambda, n);
    // Theoretical lower bound
    long max = ceil( (8+2 * epsilon) * m * (log(n) - log(k) + log(2) - log(delta))/(epsilon * epsilon));
//    cout << "\n Theoretical Maximum RR Sets is " << max << flush;
    set<int> seedSet;
    int covered;
    double maxRRMultiplier = ((1+ epsilon2) * epsilon3 * epsilon3)/((1-epsilon2) * epsilon2 * epsilon2);
    double maxIterations = log2(max/lambda);
    while(rrSets.size()<max) {
        assert(seedSet.size()==0);
        pair<vector<int>, int> nodesWithInfluence = rasCoverage.get()->findTopKNodesFromCandidatesWithoutUpdate(k, &rrSets, NULL);

        covered = nodesWithInfluence.second;
        if(covered>= lambda) {
            for (int x:nodesWithInfluence.first) {
                seedSet.insert(x);
            }
            double inependentEstimate = estimateAttitude(seedSet, delta/(3*maxIterations), maxRRMultiplier * rrSets.size());
            double currentEstimate = (double)m * covered/ (double)rrSets.size();
            if(currentEstimate<=(1+epsilon1)*inependentEstimate) {
//                cout << "\n Stopping condition met ";
//                cout << "\n Number of RR Sets" << rrSets.size();
                if(seedSet.size()>k) {
                    cout << "\n Seed set size is  " << seedSet.size();
                    assert(seedSet.size() <= k);

                }
                return seedSet;
            }
            seedSet.clear();
            seedSet.swap(seedSet);
        }
        int from = rrSets.size();
        int to = rrSets.size()*2-1;
        int previousSize=rrSets.size();
        generateNewRRSets(rrSets.size());
        int newSize = rrSets.size();
        assert(newSize==2*previousSize);
        rasCoverage.get()->addNewRRSets(&rrSets, from, to);
    }

    return seedSet;
}

double AttitudeMaximizer::estimateAttitude(std::set<int> candidateSet, double delta, int maxRRSets) {
    double lambda2 = 1 + (4*exp(1) - 8) * (1+epsilon2) * (-log(delta))/(epsilon2 * epsilon2);
    deque<int> q;
    int randomVertex;
    int n = this->graph->getNumberOfVertices();
    int m = this->graph->getNumberOfEdges();
    vector<pair<int,int>> *edgeSet = this->graph->getEdgeSet();
    pair<int,int> randomEdge;
    int randomEdgeID;
    int totalSize = 0;
    bool found;
    int covered = 0;
    for(int i=0;i<maxRRSets;i++) {
        randomEdgeID = sfmt_genrand_uint32(&sfmt) % m;
        randomEdge = (*edgeSet)[randomEdgeID];
        found = false;
        if (graph->flipCoinOnEdge(randomEdge.first, randomEdge.second)) {

            q.clear();
            randomVertex = randomEdge.first;
            q.push_back(randomVertex);
            int nVisitMark = 0;
            visitMark[nVisitMark++] = randomVertex;
            visited[randomVertex] = true;
            if(candidateSet.find(randomVertex)!=candidateSet.end()) {
                found = true;
            }
            vector<vector<int>> *graphTranspose = graph->getGraphTranspose();
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
                    if(candidateSet.find(v)!=candidateSet.end()) {
                        found = true;
                        break;
                    }
                    q.push_back(v);
                }
            }

            for(int i=0;i<nVisitMark;i++) {
                visited[visitMark[i]] = false;
            }
            if(found) covered++;

            if(covered>=lambda2) {
                return m*((double)covered)/(double)(i+1);
            }
        }
    }
    return -1;

}

void AttitudeMaximizer::generateNewRRSets(int R) {
    int n = this->graph->getNumberOfVertices();
    int m = this->graph->getNumberOfEdges();
    vector<pair<int,int>> *edgeSet = this->graph->getEdgeSet();
    pair<int,int> randomEdge;
    int randomEdgeID;
    for(int i=0;i<R;i++) {
        randomEdgeID = sfmt_genrand_uint32(&sfmt) % m;
        randomEdge = (*edgeSet)[randomEdgeID];
        if(graph->flipCoinOnEdge(randomEdge.first, randomEdge.second)) {
            generateRandomRRSet(randomEdge.first, &rrSets);
        } else {
            rrSets.push_back(vector<int>()); // The edge failed to be activated. Push back an empty rr set.
        }
    }
}
void AttitudeMaximizer::generateRandomRRSet(int randomVertex, vector<vector<int>> *rrSets) {

    q.clear();

    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    vector<vector<int>> *graphTranspose = graph->getGraphTranspose();
    while(!q.empty()) {
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
        }
    }
    rrSets->push_back(vector<int>(visitMark.begin(), visitMark.begin()+nVisitMark));
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
    }

}

int AttitudeMaximizer::getNumberOfRRSetsGenerated() {
    return (int)rrSets.size();
}
//
// Created by Madhavan R.P on 2019-06-11.
//

#include "AttitudeCalculator.hpp"


AttitudeCalculator::AttitudeCalculator(Graph *graph) {
    sfmt_init_gen_rand(&sfmt, rand());
    //Default Epsilon
    constructCalculator(graph, 2);
}

AttitudeCalculator::AttitudeCalculator(Graph *graph, double epsilon) {
    sfmt_init_gen_rand(&sfmt, rand());
    constructCalculator(graph, epsilon);
}

void AttitudeCalculator::constructCalculator(Graph *graph, double epsilon) {
    this->graph = graph;
    this->epsilon = epsilon;
    int m = graph->getNumberOfEdges();
    int n = graph->getNumberOfVertices();
    //Initialize RR Set related data structures
    for(int i=0;i<n;i++) {
        this->targetCounts.push_back(0);
        this->visitMark.push_back(0);
        this->visited.push_back(false);
    }
    int R = (8+2 * epsilon) * m * (2 * log(m) + log(2))/(epsilon * epsilon);
//    R = (double)0.15*(double)R;

    // Generate the Random RR Sets
    generateRandomRRSets(R);
    this->lookupTableTargets.reset(new vector<vector<int>>());

    this->timCoverageTargets.reset(new TIMCoverage(this->lookupTableTargets.get()));

    this->timCoverageTargets->initializeLookupTable(&rrSetsTargets, n);
    this->timCoverageTargets->initializeDataStructures(R, n);

}

void AttitudeCalculator::generateRandomRRSets(int R) {
    int n = this->graph->getNumberOfVertices();
    int m = this->graph->getNumberOfEdges();
    vector<pair<int,int>> *edgeSet = this->graph->getEdgeSet();
    pair<int,int> randomEdge;
    int randomEdgeID;
    int totalSize = 0;
    cout << "\n Generating number of RR Sets: " << R << flush;
    for(int i=0;i<R;i++) {
        randomEdgeID = sfmt_genrand_uint32(&sfmt) % m;
        randomEdge = (*edgeSet)[randomEdgeID];
        if(graph->flipCoinOnEdge(randomEdge.first, randomEdge.second)) {
            generateRandomRRSet(randomEdge.first, &rrSetsTargets, &targetCounts);
        } else {
            rrSetsTargets.push_back(vector<int>()); // The edge failed to be activated. Push back an empty rr set.
        }
        totalSize += (int)rrSetsTargets[i].size();
//        if(i%10000==0) {
//            cout << "\n Finished number of sets: " << i << flush;
//        }
    }
    cout << "\n Total Number of elements in RR Sets: " << totalSize;
    cout << "\n Average size of an RR Set is " << (double)totalSize/(double)R;
}

void AttitudeCalculator::generateRandomRRSet(int randomVertex, vector<vector<int>> *rrSets, vector<int> *counts) {

    q.clear();

    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    (*counts)[randomVertex]++;
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
            (*counts)[v]++;
            q.push_back(v);
        }
    }
    rrSets->push_back(vector<int>(visitMark.begin(), visitMark.begin()+nVisitMark));
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
    }

}

double AttitudeCalculator::getScalingFactor() {
    double scalingFactor = (double)this->graph->getNumberOfEdges()/(int)this->rrSetsTargets.size();
    return scalingFactor;
}

int AttitudeCalculator::findAttitude(set<int> seedSet) {
    for (int seed:seedSet) {
        this->timCoverageTargets->addToSeed(seed, &this->rrSetsTargets);
    }
    int count = 0;
    for(bool edge:this->timCoverageTargets->edgeMark) {
        if(edge) count++;
    }
    int attitude = round((double) count * getScalingFactor());
    return attitude;
}

vector<vector<int>>* AttitudeCalculator::getRRsets() {
    return &this->rrSetsTargets;
}

shared_ptr<TIMCoverage> AttitudeCalculator::getTIMCoverage() {
    return timCoverageTargets;
}


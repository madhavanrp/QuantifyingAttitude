//
// Created by Madhavan Rajagopal Padmanabhan on 2020-01-13.
//

#include "AttitudeMinusInfMaximizer.hpp"


AttitudeMinusInfMaximizer::AttitudeMinusInfMaximizer(Graph *graph) {
    sfmt_init_gen_rand(&sfmt, rand());
    this->graph = graph;
    for(int i=0;i<graph->getNumberOfVertices();i++) {
        this->visitMark.push_back(0);
        this->visited.push_back(false);
    }

    //RAS Data Structure
    this->lookupTable.reset(new vector<vector<int>>());
    this->rasCoverage.reset(new RISRRGraph(this->lookupTable.get()));

}

//void AttitudeMinusInfMaximizer::performDFS(RandomGraph &g, int startVertex) {
//    // g is the reversed graph
//    stack<int> dfsStack;
//    dfsStack.push(startVertex);
//    int nVisitMark = 0;
//    visitMark[nVisitMark++] = startVertex;
//    visited[startVertex] = true;
//
//    while(!dfsStack.empty()) {
//        int u=dfsStack.top();
//        dfsStack.pop();
//        for(int j=0; j<(int)g[u].second.size(); j++){
//            int v = g[u].second[j];
//
//            if(visited[v]) {
//                // The vertex's value is incremented only when there is another path to it.
//                // This will make v's value to be (attitude due to v - 1) where 1 comes from influencing the start vertex.
//                g[v].first++;
//                continue;
//            }
//
//            visitMark[nVisitMark++]=v;
//            visited[v]=true;
//            dfsStack.push(v);
//        }
//    }
////    cout << "\n";
//
////    for(int i=0;i<nVisitMark;i++) {
////        cout << "\n Visited " << visitMark[i];
////    }
////    cout << "\n Total vertices in graph " << g.size() << flush;
//    for(int i=0;i<nVisitMark;i++) {
//        visited[visitMark[i]] = false;
//    }
//}

void AttitudeMinusInfMaximizer::generateRandomRRSet(int randomVertex, vector<vector<int>> *rrSets) {

    deque<int> q;

    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    vector<vector<int>> *graphTranspose = graph->getGraphTranspose();
    RandomGraph randomGraph;
    while(!q.empty()) {
        int u=q.front();
        if(randomGraph.counts.find(u)==randomGraph.counts.end()) {
            randomGraph.counts[u] = 0;
            randomGraph.edges[u] = vector<int>();
            randomGraph.vertices.insert(u);
        }

        q.pop_front();
        for(int j=0; j<(int)(*graphTranspose)[u].size(); j++){
            int v = (*graphTranspose)[u][j];
            if(!graph->flipCoinOnEdge(v, u))
                continue;

            randomGraph.edges[u].push_back(v);

            if(visited[v])
                continue;

            visitMark[nVisitMark++]=v;
            visited[v]=true;
            q.push_back(v);
        }
    }


    rrSets->push_back(vector<int>(visitMark.begin(), visitMark.begin()+nVisitMark));
    for(int i=0;i<nVisitMark;i++) {
        assert(visited[visitMark[i]]);
        visited[visitMark[i]] = false;
    }

    randomGraphs.push_back(randomGraph);
    startVertices.push_back(randomVertex);

}



void AttitudeMinusInfMaximizer::generateInitialSets() {
    // For each vertex generate N_SAMPLES graph starting from that point.

    int N_SAMPLES;
    double inDegree;
    double epsilon = 0.1;
    int n = graph->getNumberOfVertices();
    for(int i=0; i<n; i++) {
        inDegree = (double)graph->inDegree[i];
        N_SAMPLES = round(inDegree/(epsilon*epsilon));
        for (int j = 0; j < N_SAMPLES; ++j) {
            generateRandomRRSet(i, &rrSets);
        }
    }
    assert(randomGraphs.size()>0);
//    assert(randomGraphs.size()==(N_SAMPLES*n));
    this->rasCoverage.get()->initializeFromRRGraphs(&randomGraphs, startVertices, n);
}

set<int> AttitudeMinusInfMaximizer::findMaximizingSeed(int k) {
    return this->rasCoverage.get()->findTopKNodes(k);
}

vector<int> AttitudeMinusInfMaximizer::getOrderedSeed() {
    return this->rasCoverage.get()->getOrderedSeed();
}

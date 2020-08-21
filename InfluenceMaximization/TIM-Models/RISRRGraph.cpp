//
// Created by Madhavan Rajagopal Padmanabhan on 2020-01-14.
//

#include "RISRRGraph.hpp"

void RISRRGraph::performBFS(RandomGraph &g, int startVertex, int vertexRemoved) {
    // g is the reversed graph
    deque<int> q;
    q.push_back(startVertex);
    set<int> visited;
    visited.insert(startVertex);
    while(!q.empty()) {
        int u=q.front();
        g.counts[u]++;
        q.pop_front();
        for(int j=0; j<(int)g.edges[u].size(); j++){
            int v = g.edges[u][j];
//            if(v==vertexRemoved) continue;
            if(this->seedSet.find(v)!=this->seedSet.end()) continue;
            if(visited.find(v)!=visited.end()) continue;

            visited.insert(v);
            q.push_back(v);
        }
    }
}

void RISRRGraph::initializeFromRRGraphs(vector<RandomGraph> *randomGraphs, vector<int> startVertices, int n) {

    this->R = (int) randomGraphs->size();
    counts = vector<int>(n);
    this->randomGraphs = randomGraphs;
    this->startVertices = startVertices;
    calculateRandomGraphCounts();
    for(RandomGraph g: *randomGraphs) {
        for(auto iter=g.counts.begin(); iter!=g.counts.end(); iter++) {

            counts[iter->first]+= iter->second;
        }
    }
    for(int i=0;i<n; i++) {
        (*lookupTable).push_back(vector<int>());
    }
    RandomGraph* randomGraph;
    for(int rrSetID=0; rrSetID<randomGraphs->size();rrSetID++) {
        randomGraph = &(*randomGraphs)[rrSetID];
        for(int v: randomGraph->vertices) {
            (*lookupTable)[v].push_back(rrSetID);
        }
    }
    initializeDataStructures(randomGraphs->size(), n);
}

void RISRRGraph::initializeDataStructures(int R, int n) {
    int numberCovered;
    for (int i = 0; i < n; i++) {
        nodeMark.push_back(false);
        coverage.push_back(0);
    }
    for (int i = 0; i < R; i++) {
        edgeMark.push_back(false);
    }


    for (int i = 0; i < n; i++) {
        numberCovered = counts[i];
        queue.push(make_pair(i, numberCovered));
        coverage[i] = numberCovered;
        nodeMark[i] = true;
    }

}

pair<int, double> RISRRGraph::findMaxInfluentialNode() {

    vector<bool> *nodeMark = &this->nodeMark;
    int maximumGainNode = -1;
    int maximumGain = 0;

    for(int i=0; i<nodeMark->size(); i++) {
        if(this->seedSet.find(i)!=this->seedSet.end()) continue;
        if(counts[i]>maximumGain) {
            maximumGain = counts[i];
            maximumGainNode = i;
        }
    }
//    cout << "\n Best node is  " << maximumGainNode << " with count " << counts[maximumGainNode] << flush;
    assert(maximumGain==counts[maximumGainNode]);


    return make_pair(maximumGainNode, maximumGainNode);
}

void RISRRGraph::calculateRandomGraphCounts() {
    for(int graphID=0; graphID<this->R; graphID++) {
        RandomGraph *randomGraph = &(*randomGraphs)[graphID];
        int startVertex = startVertices[graphID];
        for(int vertex: randomGraph->vertices) {
            randomGraph->counts[vertex] = -1;
        }

        randomGraph->counts[startVertex] = 0;
        for (int neighbor: randomGraph->edges[startVertex]) {
            performBFS(*randomGraph, neighbor, -1);
        }

    }
}

void RISRRGraph::removeVertexAndRecalculate(RandomGraph &randomGraph, int randomGraphID, int vertexToRemove) {
    // TODO: Update counts instance variable to reflect the new difference.
    // Should the counts[vertexToRemove] be deducted by the value of randomGraph.counts
    this->counts[vertexToRemove]-= randomGraph.counts[vertexToRemove];
    randomGraph.counts.erase(vertexToRemove);
    randomGraph.edges.erase(vertexToRemove);
    randomGraph.vertices.erase(vertexToRemove);
    assert(randomGraph.edges.find(vertexToRemove)==randomGraph.edges.end());

    int startVertex = startVertices[randomGraphID];
    unordered_map<int, int> temporaryCounts;
    for(int vertex: randomGraph.vertices) {
        temporaryCounts[vertex] = randomGraph.counts[vertex];
        randomGraph.counts[vertex] = 0;

    }
    randomGraph.counts[startVertex] = 0;
//    for(int vertex: randomGraph.vertices) {
//        assert(randomGraph.counts[vertex]==0);
//        assert(temporaryCounts[vertex]>=randomGraph.counts[vertex]);
//    }
    for (int neighbor: randomGraph.edges[startVertex]) {
        // Do not start a search from neighbor if it's in the seed
        if(this->seedSet.find(neighbor)!=this->seedSet.end()) continue;
        performBFS(randomGraph, neighbor, vertexToRemove);
    }

    for(int vertex: randomGraph.vertices) {
        if(randomGraph.counts[vertex]>0) randomGraph.counts[vertex]--;
        assert(randomGraph.counts.find(vertex)!=randomGraph.counts.end());
        assert(temporaryCounts.find(vertex)!=temporaryCounts.end());
        assert(this->seedSet.find(vertex)==this->seedSet.end());
        if(temporaryCounts[vertex]<randomGraph.counts[vertex]) {
            string errorMessage = "\n Error. V1 " + to_string(temporaryCounts[vertex]) + " and V2 " + to_string(randomGraph.counts[vertex]);
            cout << errorMessage << flush;
        }
        assert(temporaryCounts[vertex]>=randomGraph.counts[vertex]);
        counts[vertex]-= (temporaryCounts[vertex]-randomGraph.counts[vertex]);
    }
}

void RISRRGraph::addToSeed(int vertex) {
//    cout << "\n Before adding vertex " << vertex;
//    cout << "\n Count of 1821 is " << counts[1821];
//    cout << "\n Count of 107 is " << counts[107];
    vector<int> *graphIDs = &(*this->lookupTable)[vertex];
    this->seedSet.insert(vertex);
    for(int gID: *graphIDs) {
        RandomGraph *randomGraph = &(*randomGraphs)[gID];
        removeVertexAndRecalculate(*randomGraph, gID, vertex);
    }
//    cout << "\n After adding vertex " << vertex;
//    cout << "\n Count of 1821 is " << counts[1821];
//    cout << "\n Count of 107 is " << counts[107];
//    cout << flush;
//    cout << "\n Sorted numbers";
//    vector<int> copy = vector<int>(this->counts);
//    sort(copy.begin(), copy.end(), greater<int>());
//    for(auto num: copy) {
//        cout << " " << num;
//    }
//    cout << flush;

}

set<int> RISRRGraph::findTopKNodes(int k) {
    set<int> topKNodes;
    int u;
    for(int i=0; i< k; i++) {
        int u = findMaxInfluentialNode().first;
        if(u==-1) break;
        topKNodes.insert(u);
        addToSeed(u);
        orderedSeed.push_back(u);
    }
    return topKNodes;
}

vector<int> RISRRGraph::getOrderedSeed() {
    return orderedSeed;
}
//
//  TIMCoverage.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 1/16/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#include "TIMCoverage.hpp"
TIMCoverage::TIMCoverage(vector<vector<int>> *lookupTable) {
    this->lookupTable = lookupTable;
    this->numberOfRRSetsCovered = 0;
}

void TIMCoverage::decrementCountForVertex(int u, int setID) {
    (*lookupTable)[u].erase(std::remove((*lookupTable)[u].begin(), (*lookupTable)[u].end(), setID), (*lookupTable)[u].end());
}

void TIMCoverage::incrementCountForVertex(int u, int randomRRSetID) {
    (*lookupTable)[u].push_back(randomRRSetID);
}

inline int TIMCoverage::countForVertex(int u) {
    assert(u<this->lookupTable->size() && u>=0);
    return (int)(*lookupTable)[u].size();
}

vector<int> TIMCoverage::getRRSetsCoveredByVertex(int vertex) {
    return (*lookupTable)[vertex];
}

int TIMCoverage::numberOfNewRRSetsCoveredByVertex(int vertex) {
    return this->coverage[vertex];
}

double TIMCoverage::marginalGainWithVertex(int vertex, double scalingFactor) {
    return (this->coverage[vertex] * scalingFactor);
}

void TIMCoverage::offsetCoverage(int vertex, int offset) {
    this->coverage[vertex] = this->coverage[vertex] + offset;
}

void TIMCoverage::initializeLookupTable(vector<vector<int>>* randomRRSets, int n) {
    
    for(int i=0;i<n; i++) {
        (*lookupTable).push_back(vector<int>());
    }
    
    for(int rrSetID=0; rrSetID<randomRRSets->size();rrSetID++) {
        
        for(int vertex: (*randomRRSets)[rrSetID]) {
            incrementCountForVertex(vertex, rrSetID);
        }
    }
}

void TIMCoverage::updatePriorityQueueWithCurrentValues() {
//    this->queue = priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator>();
    while (!this->queue.empty()) {
        this->queue.pop();
    }
    
    for (int i=0; i<this->coverage.size(); i++) {
        this->queue.push(make_pair(i, this->coverage[i]));
    }
}

void TIMCoverage::constructReverseQueue() {
    while(!this->reverseQueue.empty()) {
        this->reverseQueue.pop();
    }
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *queueCopy = new priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator>(this->queue);
    while (!queueCopy->empty()) {
        this->reverseQueue.push(queueCopy->top());
        queueCopy->pop();
    }
    delete queueCopy;
}

set<int> TIMCoverage::findMinInfluentialNodes(vector<vector<int>> *rrSets) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, ReverseQueueComparator> *queue = new priority_queue<pair<int, int>, vector<pair<int, int>>, ReverseQueueComparator>(this->reverseQueue);
    vector<int> *coverage = &this->coverage;
    vector<bool> *nodeMark = &this->nodeMark;
    int minimumGainNode = -1;
    int influence = 0;
    set<int> minNodes;
    int previousInfluence = -1;
    while(!queue->empty()) {
        pair<int,int> element = queue->top();
        if(element.second > (*coverage)[element.first]) {
            queue->pop();
            element.second = (*coverage)[element.first];
            queue->push(element);
            continue;
        }
        
        queue->pop();
        if(!(*nodeMark)[element.first]) {
            continue;
        }
        
        minimumGainNode = element.first;
        influence = (*coverage)[element.first];
        
        if (previousInfluence==-1) {
            previousInfluence = influence;
        }
        if (previousInfluence!=influence) {
            break;
        }
        minNodes.insert(minimumGainNode);
        
        previousInfluence = influence;
    }
    delete queue;
    return minNodes;
}

void TIMCoverage::initializeDataStructures(int R, int n) {
    int numberCovered;
    for (int i = 0; i < n; i++) {
        nodeMark.push_back(false);
        coverage.push_back(0);
    }
    for (int i = 0; i < R; i++) {
        edgeMark.push_back(false);
    }
    for (int i = 0; i < n; i++) {
        
        numberCovered = this->countForVertex(i);
        queue.push(make_pair(i, numberCovered));
        coverage[i] = numberCovered;
        nodeMark[i] = true;
    }
    this->R = R;
    
}

pair<int, double> TIMCoverage::findMaxInfluentialNodeAndUpdateModel(vector<vector<int>> *rrSets, NodeChecker *nodeChecker) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *queue = &this->queue;
    
    vector<int> *coverage = &this->coverage;
    vector<bool> *nodeMark = &this->nodeMark;
    int maximumGainNode = -1;
    int influence = 0;
    while(!queue->empty()) {
        pair<int,int> element = queue->top();
        if(element.second > (*coverage)[element.first]) {
            queue->pop();
            element.second = (*coverage)[element.first];
            queue->push(element);
            continue;
        }
        
        queue->pop();
        if (nodeChecker!=NULL && !nodeChecker->isNodeValid(element.first)) {
            continue;
        }
        if(!(*nodeMark)[element.first]) {
            continue;
        }
        
        maximumGainNode = element.first;
        influence = (*coverage)[element.first];
        break;
    }
    
    int R = this->R;
    double scaledInfluence = (double) influence * nodeMark->size()/R;
    
    vector<bool> *edgeMark = &this->edgeMark;
    (*nodeMark)[maximumGainNode] = false;
    int numberCovered = this->countForVertex(maximumGainNode);
    vector<int> edgeInfluence = (*this->lookupTable)[maximumGainNode];
    
    for (int i = 0; i < numberCovered; i++) {
        if ((*edgeMark)[edgeInfluence[i]]) continue;
        
        vector<int> nList = (*rrSets)[edgeInfluence[i]];
        for (int l :
             nList) {
            if ((*nodeMark)[l]) {
                (*coverage)[l]--;
            }
        }
        (*edgeMark)[edgeInfluence[i]] = true;
        this->numberOfRRSetsCovered++;
    }
    
    return make_pair(maximumGainNode, scaledInfluence);
}

set<int> TIMCoverage::findTopKNodesModular(int k) {
    int i = 0;
    set<int> seedSet;
    while(i<k) {
        seedSet.insert(this->queue.top().first);
        this->queue.pop();
        i++;
    }
    return seedSet;
}

pair<int, double> TIMCoverage::findMaxInfluentialNodeAndUpdateModel(vector<vector<int>> *rrSets) {
    return findMaxInfluentialNodeAndUpdateModel(rrSets, NULL);
}

pair<int, double> TIMCoverage::findMaxInfluentialNodeWithApproximations(set<int> *seedSet, vector<int> *approximationsScaled) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *queueCopy = new priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator>(this->queue);
    int maxValue = -1;
    int maximumGainNode = -1;
    while(!queueCopy->empty()) {
        pair<int, int> topElement = queueCopy->top();
        queueCopy->pop();
        int scaledApproximation = (*approximationsScaled)[topElement.first];
        if(this->coverage[topElement.first] - scaledApproximation > maxValue) {
            if(seedSet->find(topElement.first)==seedSet->end()) {
                maxValue = this->coverage[topElement.first] - scaledApproximation;
                maximumGainNode = topElement.first;
            }
        }
    }
    
    int R = this->R;
    double scaledInfluence = (double) maxValue * this->nodeMark.size()/R;
    delete queueCopy;
    
    return make_pair(maximumGainNode, scaledInfluence);
    
}

set<pair<int, int>> TIMCoverage::findTopKNodesWithInfluence(int k, vector<vector<int>> *rrSets) {
    set<pair<int, int>> topKNodesWithInfluence;
    for(int i=0; i< k; i++) {
        topKNodesWithInfluence.insert(findMaxInfluentialNodeAndUpdateModel(rrSets));
    }
    return topKNodesWithInfluence;
}

set<int> TIMCoverage::findTopKNodes(int k, vector<vector<int>> *rrSets) {
    set<int> topKNodes;
    for(int i=0; i< k; i++) {
        topKNodes.insert(findMaxInfluentialNodeAndUpdateModel(rrSets).first);
    }
    return topKNodes;
}

pair<vector<int>,int> TIMCoverage::findTopKNodesFromCandidatesWithoutUpdate(int k, vector<vector<int>> *rrSets, set<int> *candidateNodes) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *queue = new priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator>(this->queue);
    
    vector<int> *coverage = new vector<int>(this->coverage);
    vector<bool> *nodeMark = new vector<bool>(this->nodeMark);
    vector<bool> *edgeMark = new vector<bool>(this->edgeMark);
    int maximumGainNode = -1;
    vector<int> topKNodes;
    int rrSetsCovered = 0;
    while(!queue->empty() && topKNodes.size()<k) {
        pair<int,int> element = queue->top();
        if(element.second > (*coverage)[element.first]) {
            queue->pop();
            element.second = (*coverage)[element.first];
            queue->push(element);
            continue;
        }
        
        queue->pop();
        if(candidateNodes!=NULL) {
            if (candidateNodes->find(element.first)==candidateNodes->end()) {
                continue;
            }
        }
        if(!(*nodeMark)[element.first]) {
            continue;
        }
        topKNodes.push_back(element.first);
        
        int numberCovered = this->countForVertex(element.first);
        vector<int> edgeInfluence = (*this->lookupTable)[element.first];
        
        for (int i = 0; i < numberCovered; i++) {
            if ((*edgeMark)[edgeInfluence[i]]) continue;
            
            vector<int> nList = (*rrSets)[edgeInfluence[i]];
            for (int l :
                 nList) {
                if ((*nodeMark)[l]) {
                    (*coverage)[l]--;
                }
            }
            (*edgeMark)[edgeInfluence[i]] = true;
            rrSetsCovered++;
        }
    }
    
    
    delete coverage;
    delete nodeMark;
    delete edgeMark;
    delete queue;
    return make_pair(topKNodes, rrSetsCovered);
}

void TIMCoverage::addToSeed(int vertex, vector<vector<int>> *rrSets) {
    
    vector<int> *coverage = &this->coverage;
    vector<bool> *nodeMark = &this->nodeMark;
    vector<bool> *edgeMark = &this->edgeMark;
    (*nodeMark)[vertex] = false;
    int numberCovered = this->countForVertex(vertex);
    vector<int> edgeInfluence = (*this->lookupTable)[vertex];
    
    for (int i = 0; i < numberCovered; i++) {
        if ((*edgeMark)[edgeInfluence[i]]) continue;
        
        vector<int> nList = (*rrSets)[edgeInfluence[i]];
        for (int l :
             nList) {
            if ((*nodeMark)[l]) {
                (*coverage)[l]--;
            }
        }
        (*edgeMark)[edgeInfluence[i]] = true;
        this->numberOfRRSetsCovered++;
    }
}

double TIMCoverage::findInfluence(set<int> seedSet, double scalingFactor) {
    // Should not update anything
    // For each vertex in seed set:
    //  Go to each rr set covered. Mark edgeMark[rrSet Covered] as true
    // Go through edgeMark and find number of trues. Scale it as per scaling factor and return influence.
    vector<int> edgeMarkTemporary(this->edgeMark.size(), false);
    int rrSetsCovered = 0;
    for(int seed:seedSet) {
        vector<int> *edgeInfluence = &(*this->lookupTable)[seed];
        for (int rrSetID: *edgeInfluence) {
            if(edgeMarkTemporary[rrSetID]) continue;
            edgeMarkTemporary[rrSetID] = true;
            rrSetsCovered++;
        }
    }
//    return round((double)rrSetsCovered * scalingFactor);
    return rrSetsCovered * scalingFactor;
}

vector<double> TIMCoverage::singleNodeMarginalGainWRTSet(vector<int> X, double scalingFactor) {
    vector<double> marginalGainWRTSet(X.size());
//    unordered_map<int, int> unmap;
//    unmap[1] = 2;
//    for (int i=0; i<X.size(); i++) {
//        marginalGainWRTSet[i] = rand() % 10;
//    }
//    for (int seed:X) {
//        vector<int> someList = (*this->lookupTable)[seed];
//    }
//    return marginalGainWRTSet;
    
    vector<bool> edgeMarkTemporary(this->edgeMark.size(), false);
    unordered_map<int, int> rrSetToVertices;
    int rrSetsCovered = 0;
    vector<int> edgeInfluence;
    for(int seed:X) {
        assert(seed<this->nodeMark.size()&& seed>=0);
        edgeInfluence = (*this->lookupTable)[seed];
        for (int rrSetID: edgeInfluence) {
            assert(rrSetID>=0 && rrSetID<this->edgeMark.size());
            if(rrSetToVertices.find(rrSetID)==rrSetToVertices.end()) {
                rrSetToVertices[rrSetID] = 0;
            }
            rrSetToVertices[rrSetID] = rrSetToVertices[rrSetID]+1;
            if(edgeMarkTemporary[rrSetID]) continue;
            edgeMarkTemporary[rrSetID] = true;
            rrSetsCovered++;
        }
    }
    assert(rrSetToVertices.size()==rrSetsCovered);
    int marginalGain, seed;
    for (int i=0; i<X.size(); i++) {
        seed = X[i];
        assert(seed<this->nodeMark.size() && seed>=0);
        marginalGain = 0;
        edgeInfluence = (*this->lookupTable)[seed];
        for (int rrSetID: edgeInfluence) {
            assert(rrSetID>=0 && rrSetID<this->edgeMark.size());
            assert(rrSetToVertices.find(rrSetID)!=rrSetToVertices.end());
            if(rrSetToVertices[rrSetID] == 1) {
                marginalGain++;
            }
        }
        marginalGainWRTSet[i] = marginalGain * scalingFactor;
    }
    return marginalGainWRTSet;
}

int TIMCoverage::getNumberOfRRSetsCovered() {
    return this->numberOfRRSetsCovered;
}

void TIMCoverage::incrementRRSetsCovered(int number) {
    this->numberOfRRSetsCovered = this->numberOfRRSetsCovered + number;
}

vector<bool>* TIMCoverage::getNodeMark() {
    return &nodeMark;
}
vector<bool>* TIMCoverage::getEdgeMark() {
    return &edgeMark;
}

void TIMCoverage::addNewRRSets(vector<vector<int>> *rrSets, int from, int to) {
    int numberOfNewRRSets = to-from+1;
    for (int i = from; i < (to+1); ++i) {
        edgeMark.push_back(false);
        for (int u: (*rrSets)[i]) {
            (*lookupTable)[u].push_back(i);
            this->coverage[u]++;
        }
    }
    updatePriorityQueueWithCurrentValues();
    assert(rrSets->size()==edgeMark.size());
    assert(lookupTable->size()==nodeMark.size());
}

TIMCoverage::TIMCoverage( const TIMCoverage &obj) {
    
    queue = obj.queue;
    lookupTable = obj.lookupTable;
    R = obj.R;
    numberOfRRSetsCovered = obj.numberOfRRSetsCovered;
    for(bool x: obj.nodeMark) {
        nodeMark.push_back(x);
    }
    
    for(bool x: obj.edgeMark) {
        edgeMark.push_back(x);
    }
    for(int x: obj.coverage) {
        coverage.push_back(x);
    }
}

TIMCoverage& TIMCoverage::operator=( const TIMCoverage &obj) {
    if (&obj==this) {
        return *this;
    }
    queue = obj.queue;
    lookupTable = obj.lookupTable;
    R = obj.R;
    numberOfRRSetsCovered = obj.numberOfRRSetsCovered;
    for(bool x: obj.nodeMark) {
        nodeMark.push_back(x);
    }
    
    for(bool x: obj.edgeMark) {
        edgeMark.push_back(x);
    }
    for(int x: obj.coverage) {
        coverage.push_back(x);
    }
    return *this;
}

TIMCoverage::~TIMCoverage() {
    this->lookupTable = NULL;
}

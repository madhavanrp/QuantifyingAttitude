//
//  TIMNormalizedCoverage.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 4/24/19.
//  Copyright © 2019 Madhavan R.P. All rights reserved.
//

#include "TIMNormalizedCoverage.hpp"

TIMNormalizedCoverage::TIMNormalizedCoverage(vector<vector<int>> *lookupTable, double scalingFactor) {
    this->lookupTable = lookupTable;
    this->numberOfRRSetsCovered = 0;
    this->cost = 0;
    this->scalingFactor = scalingFactor;
}



void TIMNormalizedCoverage::initializeLookupTable(vector<vector<int>>* randomRRSets, int n, vector<double> nonTargetsInfluenced) {
    this->nonTargetsInfluenced = nonTargetsInfluenced;
    for(int i=0;i<n; i++) {
        (*lookupTable).push_back(vector<int>());
    }
    
    for(int rrSetID=0; rrSetID<randomRRSets->size();rrSetID++) {
        
        for(int vertex: (*randomRRSets)[rrSetID]) {
            (*lookupTable)[vertex].push_back(rrSetID);
        }
    }
}


void TIMNormalizedCoverage::initializeDataStructures(int R, int n) {
    for (int i = 0; i < n; i++) {
        nodeMark.push_back(false);
        normalizedCoverage.push_back(0);
    }
    for (int i = 0; i < R; i++) {
        edgeMark.push_back(false);
    }
    double normalized;
    for (int i = 0; i < n; i++) {
        
        normalized = ((double)(*lookupTable)[i].size())/(this->nonTargetsInfluenced[i]+1);
        queue.push(make_pair(i, normalized));
        normalizedCoverage[i] = normalized;
        nodeMark[i] = true;
    }
    this->R = R;
    
}

pair<int, double> TIMNormalizedCoverage::findMaxInfluentialNode(vector<vector<int>> *rrSets) {
    priority_queue<pair<int, double>, vector<pair<int, double>>, NormalizedQueueComparator> *queue = &this->queue;
    
    vector<double> *normalizedCoverage = &this->normalizedCoverage;
    vector<bool> *nodeMark = &this->nodeMark;
    int maximumGainNode = -1;
    double normalizedGain = 0;
    int count = 0;
    while(!queue->empty()) {
        pair<int,double> element = queue->top();
        if(element.second > (*normalizedCoverage)[element.first]) {
            queue->pop();
            element.second = (*normalizedCoverage)[element.first];
            queue->push(element);
            count++;
            assert(element.second == (*normalizedCoverage)[element.first]);
//            if (count%1000==0) {
//                cout << "\n Processed node with ID: " << element.first;
//                cout << "\n Large number of comparisons" << flush;
//
//            }
            continue;
        }
        
        queue->pop();
        if(!(*nodeMark)[element.first]) {
            continue;
        }
        
        maximumGainNode = element.first;
        normalizedGain = (*normalizedCoverage)[element.first];
        break;
    }
    
    
    
    return make_pair(maximumGainNode, normalizedGain);
    
    
}

void TIMNormalizedCoverage::addToSeed(int node, vector<vector<int>> *rrSets) {
    vector<double> *normalizedCoverage = &this->normalizedCoverage;
    vector<bool> *nodeMark = &this->nodeMark;
    vector<bool> *edgeMark = &this->edgeMark;
    (*nodeMark)[node] = false;
    int numberCovered = (int)(*lookupTable)[node].size();
    vector<int> edgeInfluence = (*this->lookupTable)[node];
    
    for (int i = 0; i < numberCovered; i++) {
        if ((*edgeMark)[edgeInfluence[i]]) continue;
        
        vector<int> nList = (*rrSets)[edgeInfluence[i]];
        for (int l :
             nList) {
            if ((*nodeMark)[l]) {
                (*normalizedCoverage)[l] -= ((1)/(nonTargetsInfluenced[l]+1));
                //                (*coverage)[l]--;
            }
        }
        (*edgeMark)[edgeInfluence[i]] = true;
        this->numberOfRRSetsCovered++;
    }
    this->cost+= nonTargetsInfluenced[node];
}


set<int> TIMNormalizedCoverage::findTopKNodes(int k, vector<vector<int>> *rrSets) {
    set<int> topKNodes;
    for(int i=0; i< k; i++) {
        topKNodes.insert(findMaxInfluentialNode(rrSets).first);
    }
    return topKNodes;
}

set<int> TIMNormalizedCoverage::findTopNodes(int costConstraint, vector<vector<int>> *rrSets) {
    set<int> topKNodes;
    while (true) {
        pair<int, double> nodeWithNormalizedGain = findMaxInfluentialNode(rrSets);
//        cout << "\n Found node with normalized gain" << nodeWithNormalizedGain.first << flush;
//        cout << "\n Queue size is " << this->queue.size() << flush;
        if (nodeWithNormalizedGain.first==-1) {
//            cout << "\n Breaking" << flush;
            break;
        }
        if (this->cost+this->nonTargetsInfluenced[nodeWithNormalizedGain.first]>costConstraint) {
            continue;
        }
//        cout << "\n Adding to seed" << flush;
        addToSeed(nodeWithNormalizedGain.first, rrSets);
        topKNodes.insert(nodeWithNormalizedGain.first);
    }
    return topKNodes;
}

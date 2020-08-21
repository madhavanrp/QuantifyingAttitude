//
//  TIMCoverage.c
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 9/8/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//


#ifndef TIMCoverage_c
#define TIMCoverage_c

#include <iostream>
#include <algorithm>
#include <assert.h>
#include <queue>
#include <set>
#include <math.h>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct QueueComparator {
    bool operator()(pair<int, int> a, pair<int, int> b)
    {
        return a.second < b.second;
    }
};

struct ReverseQueueComparator {
    bool operator()(pair<int, int> a, pair<int, int> b)
    {
        return a.second > b.second;
    }
};

class NodeChecker {
public:
    virtual bool isNodeValid(int nodeID)=0;
};

class TIMCoverage {
protected:
    int numberOfRRSetsCovered;
    priority_queue<pair<int, int>, vector<pair<int, int>>, ReverseQueueComparator> reverseQueue;
public:
    vector<bool> nodeMark;
    vector<bool> edgeMark;
    vector<int> coverage;
    vector<vector<int>> *lookupTable;
    int R;
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> queue;
    
    
    TIMCoverage(vector<vector<int>> *lookupTable);
    
    void decrementCountForVertex(int u, int setID);
    
    void incrementCountForVertex(int u, int randomRRSetID);
    
    int countForVertex(int u);
    int numberOfNewRRSetsCoveredByVertex(int vertex);
    double marginalGainWithVertex(int vertex, double scalingFactor);
    void incrementRRSetsCovered(int number);
    
    vector<int> getRRSetsCoveredByVertex(int vertex);
    
    void offsetCoverage(int vertex, int offset);
    void updatePriorityQueueWithCurrentValues();
    
    void initializeLookupTable(vector<vector<int>>* randomRRSets, int n) ;
    
    void initializeDataStructures(int R, int n) ;
    
    pair<int, double> findMaxInfluentialNodeAndUpdateModel(vector<vector<int>> *rrSets) ;
    pair<int, double> findMaxInfluentialNodeAndUpdateModel(vector<vector<int>> *rrSets, NodeChecker *nodeChecker);
    set<int> findMinInfluentialNodes(vector<vector<int>> *rrSets);
    
    pair<int, double> findMaxInfluentialNodeWithApproximations(set<int> *seedSet, vector<int> *approximationsScaled);
    
    set<pair<int, int>> findTopKNodesWithInfluence(int k, vector<vector<int>> *rrSets);
    pair<vector<int>,int> findTopKNodesFromCandidatesWithoutUpdate(int k, vector<vector<int>> *rrSets, set<int> *candidateNodes);
    void constructReverseQueue();
    
    set<int> findTopKNodes(int k, vector<vector<int>> *rrSets);
    set<int> findTopKNodesModular(int k);
    
    void addToSeed(int vertex, vector<vector<int>> *rrSets);

    double findInfluence(set<int> seedSet, double scalingFactor);
    vector<double> singleNodeMarginalGainWRTSet(vector<int> X, double scalingFactor);
    int getNumberOfRRSetsCovered();
    
    vector<bool> *getNodeMark();
    vector<bool> *getEdgeMark();
    void addNewRRSets(vector<vector<int>> *rrSets, int from, int to);
    TIMCoverage( const TIMCoverage &obj);
    TIMCoverage& operator=( const TIMCoverage &obj);
    ~TIMCoverage();
};


#endif /* IMTree_hpp */

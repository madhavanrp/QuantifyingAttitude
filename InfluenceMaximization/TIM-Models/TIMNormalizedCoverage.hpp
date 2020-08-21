//
//  TIMNormalizedCoverage.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 4/24/19.
//  Copyright © 2019 Madhavan R.P. All rights reserved.
//

#ifndef TIMNormalizedCoverage_hpp
#define TIMNormalizedCoverage_hpp

#include <stdio.h>

#include <iostream>
#include <algorithm>
#include <assert.h>
#include <queue>
#include <set>
#include <math.h>

using namespace std;

struct NormalizedQueueComparator {
    bool operator()(pair<int, double> a, pair<int, double> b)
    {
        return a.second < b.second;
    }
};

class TIMNormalizedCoverage {
    int numberOfRRSetsCovered;
    vector<bool> nodeMark;
    vector<bool> edgeMark;
    vector<double> normalizedCoverage;
    vector<vector<int>> *lookupTable;
    double scalingFactor;
    double cost;
    
    int R;
    priority_queue<pair<int, double>, vector<pair<int, double>>, NormalizedQueueComparator> queue;
    vector<double> nonTargetsInfluenced;
public:
    
    
    TIMNormalizedCoverage(vector<vector<int>> *lookupTable, double scalingFactor);
    void initializeLookupTable(vector<vector<int>>* randomRRSets, int n, vector<double> nonTargetsInfluenced) ;
    void initializeDataStructures(int R, int n) ;
    pair<int, double> findMaxInfluentialNode(vector<vector<int>> *rrSets);
    set<int> findTopKNodes(int k, vector<vector<int>> *rrSets);
    void addToSeed(int vertex, vector<vector<int>> *rrSets);
    set<int> findTopNodes(int costConstraint, vector<vector<int>> *rrSets);
};
#endif /* TIMNormalizedCoverage_hpp */

//
//  Graph.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/4/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef Graph_hpp
#define Graph_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <queue>
#include <ctime>
#include <deque>
#include <string.h>
#include <stdexcept>
#include <unordered_map>
#include <set>
#include "LabelSetting.hpp"
#include "SFMT/SFMT.h"

using namespace std;

enum NodeLabel {
    NodeLabelTarget,
    NodeLabelNonTarget
};

class Graph {
private:
    float propogationProbability;
    int propogationProbabilityNumber;
    bool standardProbability;
    bool trivalency;
    sfmt_t sfmt;
    string diffusionModel;
    string graphName;
    float percentageTargets;
    vector<int> nonTargets;
    int numberOfTargets;
    int numberOfNonTargets;
    LabelSetting labelSetting;
    
    int n, m;
    vector<vector<int> > graph;
    vector<vector<int> > graphTranspose;
    vector<pair<int, int>> edgeSet; //store all edges in graph, used as the start of generating a RRSet
    unordered_map<string, double> edgeProbabilities;
    bool edgeProbabilitiesAssigned;
    
    vector<NodeLabel> labels;

    struct PairComparator {
        bool operator()(pair<int, int> a, pair<int, int> b)
        {
            return a.second < b.second;
        }
    };
public:
    Graph();
    vector<vector<int>> rrSets;
    
    deque<int> q;
    vector<int> inDegree;
    vector<bool> visited;
    vector<int> visitMark;
    void readGraph(string fileName);
    void readGraph(string fileName, float percentage);
    void readGraph(string fileName, float percentage, LabelSetting labelSetting);
    
    //Labels
    bool isTarget(int v);
    bool isNonTarget(int v);
    void readLabels(string fileName);
    void writeLabels(LabelSetting labelSetting);
    void writeLabels(LabelSetting labelSetting, string comment);
    void setLabels(vector<NodeLabel> labels, float percentageTargets);
    static string constructLabelFileName(string graphName, float percentageTargets);
    static string constructLabelFileName(string graphName, float percentageTargets, LabelSetting setting);
    
    //Numbers
    int getNumberOfVertices();
    int getNumberOfEdges();
    int getNumberOfTargets();
    int getNumberOfNonTargets();
    
    //Data Structure
    vector<int> *getNonTargets();
    vector<vector<int>> *getGraph();
    vector<vector<int>> *getGraphTranspose();
    vector<pair<int,int>> *getEdgeSet();
    
    vector<vector<int> > constructTranspose(vector<vector<int> > aGraph);
    void generateRandomRRSets(int R);
    void generateRandomRRSet(int randomVertex);
    void clearRandomRRSets();
    vector<vector<int>>* getRandomRRSets();
    
    void assertTransposeIsCorrect();
    
    //Functions for propogation probability
    void setPropogationProbability(float p);
    bool flipCoinOnEdge(int u, int v);
    int generateRandomNumber(int u, int v);
    int getPropogationProbabilityNumber();
    double getWeightForLTModel(int u, int v);
    void setDiffusionModel(string model);
    void generateEdgeProbabilitiesTrivalencyModel();
    
    
    // For heuristics
    void generateRandomRRSetsWithoutVisitingNonTargets(int R);
    vector<int> generateRandomRRSetWithoutVisitingNonTargets(int randomVertex, int rrSetID);

    // Baselines
    set<int> findTopKOutDegreeNodes(int k);
    set<int> findRandomNodes(int k);
    
};

#endif /* Graph_hpp */




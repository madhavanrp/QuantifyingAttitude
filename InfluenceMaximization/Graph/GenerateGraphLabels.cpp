//
//  GenerateGraphLabels.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 12/15/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "GenerateGraphLabels.hpp"

void GenerateGraphLabels::initializeDataAndGenerate(Graph *graph, float percentage, LabelSetting labelSetting) {
    this->graph = graph;
    this->setting = labelSetting;
    this->percentage = percentage;
    this->totalNumberOfNonTargets = 0;
    int n = graph->getNumberOfVertices();
    this->numberOfTargetsToLabel = round((float)n * percentage);
    this->numberOfNonTargetsToLabel = n - this->numberOfTargetsToLabel;
    this->labels = vector<NodeLabel>(n);
    for (int i=0; i<n; i++) {
        this->labels[i] = NodeLabelTarget;
    }
    sfmt_init_gen_rand(&sfmt, rand());
    generate();
}
GenerateGraphLabels::GenerateGraphLabels(Graph *graph, float percentage, LabelSetting setting) {
    initializeDataAndGenerate(graph, percentage, setting);
}

GenerateGraphLabels::GenerateGraphLabels(Graph *graph, float percentage) {
    initializeDataAndGenerate(graph, percentage, LabelSettingUniform);
}

void GenerateGraphLabels::doDFSWithLabel(int currentNode, int currentDepth, int depthLimit) {
    if(currentDepth>depthLimit) return;
    if(this->labels[currentNode]==NodeLabelNonTarget) return;
    if(this->totalNumberOfNonTargets>=this->numberOfNonTargetsToLabel) return;
    this->labels[currentNode] = NodeLabelNonTarget;
    this->totalNumberOfNonTargets++;
    float probability = 0.75f;
    vector<vector<int>> *adjacencyList = this->graph->getGraph();
    for(int neighbour: (*adjacencyList)[currentNode]) {
        float randomFloat = sfmt_genrand_res53(&sfmt);
        if(randomFloat<=probability && (this->labels[currentNode]==NodeLabelNonTarget)) {
            doDFSWithLabel(neighbour, currentDepth+1, depthLimit);
        }
    }
}

void GenerateGraphLabels::generate() {
    int n = this->graph->getNumberOfVertices();
    int numberOfTargets = round((float)n * this->percentage);
    int numberOfNonTargets = n - numberOfTargets;
    string comment;
    switch (this->setting) {
        case LabelSettingClusters:
            comment = "Label Setting Clusters with depth level=" + to_string(2);
            generateWithSetting1(numberOfTargets, numberOfNonTargets);
            break;
        case LabelSettingTIMNonTargets:
            comment = "Find Influence Maximization seed set, do diffusion and label as non targets";
            generateWithTIMNonTargets(numberOfTargets, numberOfNonTargets);
            break;
        case LabelSettingUniform:
            comment = " Each node is target with probability equal to percentage";
            generateUniformRandom();
            break;
        default:
            comment = " Each node is target with probability equal to percentage";
            generateUniformRandom();
            break;
    }
    this->graph->setLabels(this->labels, this->percentage);
    this->graph->writeLabels(this->setting, comment);
}

void GenerateGraphLabels::generateUniformRandom() {
    int n = graph->getNumberOfVertices();
    for (int i=0; i<n; i++) {
        float randomFloat = sfmt_genrand_res53(&sfmt);
        if (randomFloat<=this->percentage) {
            this->labels[i] = NodeLabelTarget;
        } else {
            this->labels[i] = NodeLabelNonTarget;
        }
    }
    
}

void GenerateGraphLabels::generateWithSetting1(int numberOfTargets, int numberOfNonTargets) {
    int n = graph->getNumberOfVertices();
    vector<vector<int>> adjList = *graph->getGraph();
    cout << "\n Value of n is " << n  << flush;
    cout << "\n Number of non targets aimed is " << numberOfNonTargets;
    cout << "\n Number of  targets aimed is " << numberOfTargets;
    int level = 2;
    while(this->totalNumberOfNonTargets< this->numberOfNonTargetsToLabel) {
        int randomVertex = sfmt_genrand_uint32(&sfmt) % n;
        if(this->labels[randomVertex]==NodeLabelNonTarget) continue;
        doDFSWithLabel(randomVertex, 0, level);
    }
    int sanityCount = 0;
    for(NodeLabel l: this->labels) {
        if(l==NodeLabelNonTarget) sanityCount++;
    }
    cout << "\n Total number of non targets: " << this->totalNumberOfNonTargets;
    cout << "\n Sanity count: " << sanityCount;
    cout << "\n Value of n: " << n;
}

void GenerateGraphLabels::generateWithTIMNonTargets(int numberOfTargets, int numberOfNonTargets) {
    /*Diffusion file had the function "findActivatedSetAndInfluenceUsingDiffusion". This file is now changed to Attitude.
     * Keeping this code commented for possible reinstatement.
     * */
//    double epsilon = (double)2;
//    int n = this->graph->getNumberOfVertices();
//    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
//    Graph *graph = this->graph;
//    graph->generateRandomRRSets(R);
//    vector<vector<int>>* rrSets = graph->getRandomRRSets();
//
//    vector<vector<int>> lookupTable;
//    TIMCoverage *timCoverage = new TIMCoverage(&lookupTable);
//    timCoverage->initializeLookupTable(rrSets, n);
//    timCoverage->initializeDataStructures(R, n);
//    set<int> seedSet;
//    double scalingFactor = (double)n/(double)R;
//    while(timCoverage->getNumberOfRRSetsCovered() * scalingFactor< numberOfNonTargets ) {
//        pair<int, double> nodeWithInfluence = timCoverage->findMaxInfluentialNodeAndUpdateModel(rrSets);
//        seedSet.insert(nodeWithInfluence.first);
//    }
//    delete timCoverage;
//    set<int> activatedSet = findActivatedSetAndInfluenceUsingDiffusion(graph, seedSet, NULL).second;
//    for (int activeNode:activatedSet) {
//        this->labels[activeNode] = NodeLabelNonTarget;
//        this->totalNumberOfNonTargets++;
//    }
}

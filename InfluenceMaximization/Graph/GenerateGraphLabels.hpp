//
//  GenerateGraphLabels.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 12/15/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef GenerateGraphLabels_hpp
#define GenerateGraphLabels_hpp

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "TIM-Models/TIMCoverage.hpp"
#include "TIMInfluenceCalculator.hpp"
#include "LabelSetting.hpp"
#include "SFMT/SFMT.h"

class GenerateGraphLabels {
private:
    Graph *graph;
    sfmt_t sfmt;
    LabelSetting setting;
    float percentage;
    int numberOfTargetsToLabel, numberOfNonTargetsToLabel;
    int totalNumberOfNonTargets;
    vector<NodeLabel> labels;
    void initializeDataAndGenerate(Graph *graph, float percentage, LabelSetting labelSetting);
    void generate();
    void generateWithSetting1(int numberOfTargets, int numberOfNonTargets);
    void generateWithTIMNonTargets(int numberOfTargets, int numberOfNonTargets);
    void generateUniformRandom();
    void doDFSWithLabel(int currentNode, int currentDepth, int depthLimit);
public:
    GenerateGraphLabels(Graph *graph, float percentage, LabelSetting setting);
    GenerateGraphLabels(Graph *graph, float percentage);
};

#endif /* GenerateGraphLabels_hpp */

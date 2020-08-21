//
//  main_utils.h
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 5/6/19.
//  Copyright © 2019 Madhavan R.P. All rights reserved.
//

#ifndef main_utils_h
#define main_utils_h

#include <iostream>
#include "cxxopts.hpp"
#include <ctime>
#include "InfluenceMaximization/log.h"
#include "InfluenceMaximization/Graph/Graph.hpp"
#include "InfluenceMaximization/IMResults/IMResults.h"
#include "InfluenceMaximization/memoryusage.h"
#include "InfluenceMaximization/Graph/GenerateGraphLabels.hpp"



void setupLogger() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer,sizeof(buffer),"%d-%m-%Y-%I:%M:%S",timeinfo);
    std::string str(buffer);
    FILELog::ReportingLevel() = logDEBUG3;
    string logFileName = "logs/influence-" + str + ".log";
    FILE* log_fd = fopen( logFileName.c_str(), "w" );
    Output2FILE::Stream() = log_fd;
}

Graph *createGraphObject(cxxopts::ParseResult result) {
    string graphFile = result["graph"].as<string>();
    int percentageTargets = result["percentage"].as<int>();
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    LabelSetting labelSetting = LabelSettingUniform;
    if(result["labelMethod"].count()>0) {
        labelSetting = static_cast<LabelSetting>(result["labelMethod"].as<int>());
    }
    Graph *graph = new Graph;
    graph->readGraph(graphFile, percentageTargetsFloat, labelSetting);
    return graph;
}

void loadResultsFileFrom(cxxopts::ParseResult result) {
    // Necessary paramters
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    string algorithm = result["algorithm"].as<string>();
    IMResults::getInstance().setBudget(budget);
    IMResults::getInstance().setGraphName(graphFileName);
    IMResults::getInstance().setPercentageTargets(percentageTargets);
    IMResults::getInstance().setAlgorithm(algorithm);
    
    // Optional parameters
    if(result["threshold"].count()>0) {
        int nonTargetThreshold = result["threshold"].as<int>();
        IMResults::getInstance().setNonTargetThreshold(nonTargetThreshold);
    }
    IMResults::getInstance().setPropagationProbability("inDegree");
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        IMResults::getInstance().setPropagationProbability(probability);
    }
    if (result.count("trivalency")>0) {
        IMResults::getInstance().setPropagationProbability("trivalency");
    } else if(result.count("probabilitiesAssigned")>0) {
        IMResults::getInstance().setPropagationProbability("probabilitiesAssigned");
    }
    
    if(result.count("ntfile")>0) {
        string nonTargetsFileName = result["ntfile"].as<std::string>();
        IMResults::getInstance().setFromFile(true);
        IMResults::getInstance().setNonTargetFileName(nonTargetsFileName);
    }
    if(result.count("labelMethod")>0) {
        LabelSetting setting = static_cast<LabelSetting>(result["labelMethod"].as<int>());
        IMResults::getInstance().setLabelMethod(setting);
    }
    
    if (result.count("model")>0) {
        IMResults::getInstance().setDiffusionModel(result["model"].as<std::string>());
    } else {
        IMResults::getInstance().setDiffusionModel("IC");
    }
}

void loadGraphSizeToResults(Graph *graph) {
    IMResults::getInstance().setNumberOfVertices(graph->getNumberOfVertices());
    IMResults::getInstance().setNumberOfEdges(graph->getNumberOfEdges());
}


void generateGraphLabels(cxxopts::ParseResult result) {
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    LabelSetting labelSetting = LabelSettingUniform;
    if(result["labelMethod"].count()>0) {
        labelSetting = static_cast<LabelSetting>(result["labelMethod"].as<int>());
    }
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    Graph *graph = new Graph;
    graph->readGraph(graphFileName, 1);
    GenerateGraphLabels(graph, percentageTargetsFloat, labelSetting);
    delete graph;
}


void createLabelFileIfNotExists(cxxopts::ParseResult result) {
    string graphName = result["graph"].as<string>();
    LabelSetting labelMethod = LabelSettingUniform;
    if(result["labelMethod"].count()>0) {
        labelMethod = static_cast<LabelSetting>(result["labelMethod"].as<int>());
    }

    float percentage = (float)result["percentage"].as<int>()/(float)100;
    string labelFileName = Graph::constructLabelFileName(graphName, percentage, labelMethod);
    ifstream labelFile(labelFileName);
    if (!labelFile.good()) {
        generateGraphLabels(result);
    }

}

#endif /* main_utils_h */

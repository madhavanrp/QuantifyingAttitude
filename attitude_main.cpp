//
// Created by Madhavan R.P on 2019-06-11.
//

#include "main_utils.h"
#include "InfluenceMaximization/Graph/Graph.hpp"
#include "InfluenceMaximization/IMResults/IMResults.h"
#include "InfluenceMaximization/RISCalculation/RISCalcSpaceEfficient.hpp"
#include "InfluenceMaximization/RISCalculation/InfluenceCalcSpaceEfficient.hpp"
#include "InfluenceMaximization/Attitude/AttitudeCalculator.hpp"
#include "InfluenceMaximization/Attitude/AttitudeMaximizer.hpp"
#include "InfluenceMaximization/Attitude/AttitudeCalcSpaceEfficient.hpp"
#include "InfluenceMaximization/Attitude/AttitudeMinusInfMaximizer.hpp"

#include "InfluenceMaximization/DiffusionAlgorithms/Diffusion.h"
#include "InfluenceMaximization/DiffusionAlgorithms/AttitudeMinusInfDiffusion.hpp"

#include <fstream>

string constructResultFileName(string graphFileName, int budget, int percentageTargets) {
    string resultFileName = "results/" + graphFileName;
    resultFileName+="_" + to_string(budget);
    resultFileName+="_" + to_string(percentageTargets);
    resultFileName+="_" + to_string(rand() % 1000000);
    resultFileName+=".json";
    return resultFileName;
}

IMSeedSet getIMSeedSet(set<int> seedSet, pair<int,int> influence) {
    IMSeedSet imSeedSet;
    for(int seed: seedSet) {
        imSeedSet.addSeed(seed);
    }
    imSeedSet.setTargets(influence.first);
    imSeedSet.setNonTargets(influence.second);
    return imSeedSet;
}

inline double calculateIndependentAttitudeValue(Graph *graph, set<int> seedSet) {
    // Independent verification
    AttitudeCalcSpaceEfficient *attitudeCalcSpaceEfficient = new AttitudeCalcSpaceEfficient(graph);
    attitudeCalcSpaceEfficient->initDataStructures();
    double attitude = attitudeCalcSpaceEfficient->estimate(seedSet);
    delete attitudeCalcSpaceEfficient;
    cout << "\n Independently calculated attitude:"<< attitude;
    cout << flush;
    return attitude + seedSet.size();
}

inline double calculateIndependentInfluenceValue(Graph *graph, set<int> seedSet) {
    // Independent verification
    InfluenceCalcSpaceEfficient *influenceCalculate = new InfluenceCalcSpaceEfficient(graph);
    influenceCalculate->initDataStructures();
    double influence = influenceCalculate->estimate(seedSet);
    delete influenceCalculate;
    cout << "\n Independently calculated Influence:"<< influence;
    cout << flush;
    return influence;
}

void computeHighAttitudeNodes(Graph *graph, set<int> seedSet) {
    Diffusion diffusion(graph);
    diffusion.performDiffusion(seedSet);
    vector<pair<int, double>> highAttitude = diffusion.findTopXBeliefNodes(2000);
    IMResults::getInstance().setHighAttitudeNodes(highAttitude);
}

void executeGreedy(Graph *graph, int budget, bool computeOutput) {
    clock_t startTime = clock();

    set<int> ssaSeedSet;

    //Stop and Stare approach
    AttitudeMaximizer *attitudeMaximizer = new AttitudeMaximizer(graph);
    ssaSeedSet = attitudeMaximizer->findMaximizingSeed(budget);
    clock_t endTime = clock();
    int numberOfRRSetsGenerated = attitudeMaximizer->getNumberOfRRSetsGenerated();
    delete attitudeMaximizer;

    double totalTimeTaken = double(endTime - startTime) / CLOCKS_PER_SEC;
    cout << "\n Stop and stare approach finished." << flush;


    // Compute high Attitude Nodes
//    computeHighAttitudeNodes(graph, ssaSeedSet);



    // Store the attitude seed set.
    IMSeedSet attitudeSeedSet = getIMSeedSet(ssaSeedSet, make_pair(0, 0));
    IMResults::getInstance().addBestSeedSet(attitudeSeedSet);
    IMResults::getInstance().setNumberOfRRSets(numberOfRRSetsGenerated);
    IMResults::getInstance().setTotalTimeTaken(totalTimeTaken);

    if(computeOutput) {
        clock_t calculationStartTime = clock();
        int groundTruthAttitude = calculateIndependentAttitudeValue(graph, ssaSeedSet);
        clock_t calculationEndTime = clock();
        double calculationTimeTaken = double(calculationEndTime - calculationStartTime)/CLOCKS_PER_SEC;
        IMResults::getInstance().setAttitude(groundTruthAttitude);
        IMResults::getInstance().setEstimationTimeTaken(calculationTimeTaken);
    }

//
//    double attitude = calculateIndependentAttitudeValue(graph, ssaSeedSet);
//    double influence = calculateIndependentInfluenceValue(graph, ssaSeedSet);
//    double ratio = attitude/influence;
//    IMResults::getInstance().setAttInfRatio(ratio);
//    cout << "\n Ratio: " << (attitude/influence);
//    cout << "\n Difference: " << (attitude-influence) << flush;
    AttitudeMinusInfDiffusion attitudeMinusInfDiffusion(graph);
    attitudeMinusInfDiffusion.performDiffusion(ssaSeedSet);
    IMResults::getInstance().setAttInfRatio(attitudeMinusInfDiffusion.getRatioEstimation());
    IMResults::getInstance().setAttitude(attitudeMinusInfDiffusion.getAttitudeEstimation());
    IMResults::getInstance().setAttitudeMinusInfluence(attitudeMinusInfDiffusion.getAttMinusInfluenceEstimation());


    //Now executing the normal influence maximization
//    set<int> infMaxSeedSet;
//    TIMInfluenceCalculator *timInfluenceCalculator = new TIMInfluenceCalculator(graph, 2);
//    infMaxSeedSet = timInfluenceCalculator->getTimCoverageTargets()->findTopKNodes(budget, timInfluenceCalculator->getRRsetsTargets());
//    delete timInfluenceCalculator;
//    int attitudeBaseline = calculateIndependentAttitudeValue(graph, infMaxSeedSet);
//    int influenceValue = calculateIndependentInfluenceValue(graph, infMaxSeedSet);
//    IMSeedSet influenceSeedSet = getIMSeedSet(infMaxSeedSet, make_pair(attitudeBaseline,0));
//    IMResults::getInstance().addInfluenceSeedSet(influenceSeedSet);
//    IMResults::getInstance().setAttitudeOfInfluenceMaximizer(attitudeBaseline);
//    IMResults::getInstance().setInfluenceOfAttitudeMaximizer(influenceValue);

}

void executeTwitterValidation(Graph *graph, cxxopts::ParseResult result) {
    std::ifstream infile("/Users/madhavan/Downloads/twitter-dataset/output/belief_seeds.txt");
    int s;
    set<int> seedSet;
    while (infile >> s) {
        seedSet.insert(s);
    }
    std::ifstream infile2("/Users/madhavan/Downloads/twitter-dataset/output/high_belief.txt");
    set<int> highBeliefPeople;
    while (infile2 >> s) {
        highBeliefPeople.insert(s);
    }
    Diffusion diffusion(graph);
    diffusion.performDiffusion(seedSet);
    vector<pair<int, double>> highBeliefNodes = diffusion.findTopXBeliefNodes(1000);
    set<int> highBeliefSet;
    for(pair<int,double> nodeWithBelief: highBeliefNodes) {
        highBeliefSet.insert(nodeWithBelief.first);
    }

    set<int> intersect;
    set_intersection(highBeliefPeople.begin(),highBeliefPeople.end(),highBeliefSet.begin(),highBeliefSet.end(),
                     std::inserter(intersect,intersect.begin()));
    cout << "\n Intersection size: " << intersect.size() << flush;

    for(int x: highBeliefPeople) {
        cout << "\n Belief of << " << x << " is " << diffusion.getBeliefOfNode(x);
    }

    cout << "\n Actucal nodes";

    for(pair<int,double> nodeWithBelief: highBeliefNodes) {
        cout << "\n Belief of << " << nodeWithBelief.first << " is " << nodeWithBelief.second;
    }
}

void executeAttitudeEstimation(cxxopts::ParseResult result) {
    string seedFile = result["seedFile"].as<std::string>();
    std::ifstream i(seedFile);
    json data;
    i >> data;
    data["altered"] = true;
    IMResults::getInstance().loadFromExistingJSON(data);
    // Construct graph file
    string graphFile = data["graphName"].get<std::string>();
    json prob = data["propagationProbability"];
    Graph *graph = new Graph;

    LabelSetting labelSetting = LabelSettingUniform;
    float percentageTargetsFloat=1;
    graph->readGraph(graphFile, percentageTargetsFloat, labelSetting);
    if((prob.type()==json::value_t::number_float)) {
        double probability = prob.get<double>();
        graph->setPropogationProbability(probability);
    }

    vector<int> seedVector = data["bestSeedSet"]["seedSet"].get<std::vector<int>>();
    vector<double> greedyAttitude, greedyInfluence, greedyAttMinusInf, greedyRatio;
    set<int> seedSet;
    for(int s: seedVector) {
        seedSet.insert(s);
    }

//    computeHighAttitudeNodes(graph, greedySeedSet);

    // Attitude Minus Influence Related calculations

//    double attitude = calculateIndependentAttitudeValue(graph, greedySeedSet);
//    double influence = calculateIndependentInfluenceValue(graph, greedySeedSet);
//
//    greedyAttitude.push_back(attitude);
//    greedyInfluence.push_back(influence);
//    greedyAttMinusInf.push_back(attitude-influence);
//    double ratio = (double)(attitude)/(double)influence;
//    greedyRatio.push_back(ratio);

    AttitudeMinusInfDiffusion attitudeMinusInfDiffusion(graph);
    attitudeMinusInfDiffusion.performDiffusion(seedSet);
    data["diffusionRatio"] = attitudeMinusInfDiffusion.getRatioEstimation();
    data["attitude"] = attitudeMinusInfDiffusion.getAttitudeEstimation();
    data["attMinusInf"] = attitudeMinusInfDiffusion.getAttMinusInfluenceEstimation();
//    data["greedyAttitude"] = greedyAttitude;
//    data["greedyInfluence"] = greedyInfluence;
//    data["greedyAttMinusInf"] = greedyAttMinusInf;
//    data["greedyRatio"] = greedyRatio;
//    data["attitude"] = attitude;
    string filePath = "output/" + seedFile;

    IMResults::getInstance().writeToFile(filePath);

    delete graph;
}

void executeGreedyAttitudeMaximization(cxxopts::ParseResult result) {
    cout << "\n Executing Attitude" << flush;
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = 100;

    cout << "\n Conducting experiments for:\n";
    cout <<" Graph: " << graphFileName;
    cout << "\t Budget: " << budget;
    cout << flush;



    Graph *graph = createGraphObject(result);
    bool computeEstimation = false;
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        graph->setPropogationProbability(probability);
    }
    if(result.count("trivalency")>0) {
        bool trivalency = result["trivalency"].as<bool>();
        if(trivalency) {
            graph->generateEdgeProbabilitiesTrivalencyModel();
            computeEstimation = true;
        }
    }
    loadResultsFileFrom(result);
    loadGraphSizeToResults(graph);


    if (result.count("computeOutput")>0) {
        computeEstimation = true;
    }
    executeGreedy(graph, budget, computeEstimation);
    string resultFile = constructResultFileName(graphFileName, budget, percentageTargets);
    IMResults::getInstance().writeToFile(resultFile);
    delete graph;
}

set<int> executeDiffusionGreedy(Graph *graph, int budget) {
    int n = graph->getNumberOfVertices();
    AttitudeMinusInfDiffusion attitudeMinusInfDiffusion(graph);

    set<int> diffusionSeedSet;
    for (int i=0; i<budget; i++) {
        double maxVal = -1;
        int maxNode = -1;
        double currentVal;
        for(int j=0; j<n; j++) {
            if(diffusionSeedSet.find(j)!=diffusionSeedSet.end()) continue;
            diffusionSeedSet.insert(j);
            currentVal = attitudeMinusInfDiffusion.performDiffusion(diffusionSeedSet);
            diffusionSeedSet.erase(j);
            assert(diffusionSeedSet.size()==i);
            if(currentVal>maxVal) {
                maxVal = currentVal;
                maxNode = j;
            }
        }
        if(maxNode==-1) break;
        else {
            diffusionSeedSet.insert(maxNode);
        }
        assert(diffusionSeedSet.size()==(i+1));

        cout << "\n Finished iteration " << i << flush;

    }
    return diffusionSeedSet;
}

void executeGreedyAttMinusInfMaximization(cxxopts::ParseResult result) {

    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = 100;

    cout << "\n Executing Attitude Minus Influence" << flush;
    cout << "\n Conducting experiments for:\n";
    cout <<" Graph: " << graphFileName;
    cout << "\t Budget: " << budget;
    cout << flush;

    Graph *graph = createGraphObject(result);
    bool computeEstimation = false;
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        graph->setPropogationProbability(probability);
    }
    if(result.count("trivalency")>0) {
        bool trivalency = result["trivalency"].as<bool>();
        if(trivalency) {
            graph->generateEdgeProbabilitiesTrivalencyModel();
            computeEstimation = true;
        }
    }
    loadResultsFileFrom(result);
    loadGraphSizeToResults(graph);


    if (result.count("computeOutput")>0) {
        computeEstimation = true;
    }

    clock_t startTime = clock();
    AttitudeMinusInfMaximizer *attitudeMinusInfMaximizer = new AttitudeMinusInfMaximizer(graph);
    attitudeMinusInfMaximizer->generateInitialSets();
    set<int> seedSet = attitudeMinusInfMaximizer->findMaximizingSeed(budget);
    vector<int> orderedSeed = attitudeMinusInfMaximizer->getOrderedSeed();
    delete attitudeMinusInfMaximizer;
    clock_t endTime = clock();

    cout << "\n Finished new algorithm seed" << flush;
    assert(seedSet.size()>0);
    double output;

    double totalTime = (endTime - startTime)/CLOCKS_PER_SEC;
//    // Diffusion based Estimation
//    AttitudeMinusInfDiffusion attitudeMinusInfDiffusion(graph);
//    output = attitudeMinusInfDiffusion.performDiffusion(seedSet);
//    cout << "\n New algorithm Output is " << output;
//    cout << "\n Seed set is \n";
//    for (int s:seedSet) {
//        cout <<"  "  << s << flush;
//    }


//    set<int> greedySeedSet;
//    vector<double> greedyAttitude, greedyInfluence, greedyAttMinusInf, greedyRatio;
//    for(int s: orderedSeed) {
//        greedySeedSet.insert(s);
//        double attitude = calculateIndependentAttitudeValue(graph, greedySeedSet);
//        double influence = calculateIndependentInfluenceValue(graph, greedySeedSet);
//
//        greedyAttitude.push_back(attitude);
//        greedyInfluence.push_back(influence);
//        greedyAttMinusInf.push_back(attitude-influence);
//        double ratio = (double)(attitude)/(double)influence;
//        greedyRatio.push_back(ratio);
//    }
//
//    // Add the greedy objecive values
//    IMResults::getInstance().addGreedyObjective(greedyAttitude, "greedyAttitude");
//    IMResults::getInstance().addGreedyObjective(greedyInfluence, "greedyInfluence");
//    IMResults::getInstance().addGreedyObjective(greedyAttMinusInf, "greedyAttMinusInf");
//    IMResults::getInstance().addGreedyObjective(greedyRatio, "greedyRatio");
//    IMResults::getInstance().addGreedySeedSet(orderedSeed);

//    double attitude = calculateIndependentAttitudeValue(graph, seedSet);
//    double influence = calculateIndependentInfluenceValue(graph, seedSet);

    AttitudeMinusInfDiffusion attitudeMinusInfDiffusion(graph);
    attitudeMinusInfDiffusion.performDiffusion(seedSet);

    IMSeedSet attMinusInfSeedSet = getIMSeedSet(seedSet, make_pair(0, 0));
    IMResults::getInstance().addBestSeedSet(attMinusInfSeedSet);
    IMResults::getInstance().setTotalTimeTaken(totalTime);
    IMResults::getInstance().setAttitude(attitudeMinusInfDiffusion.getAttitudeEstimation());
    IMResults::getInstance().setInfluence(attitudeMinusInfDiffusion.getInfluenceEstimation());
    double ratio = attitudeMinusInfDiffusion.getRatioEstimation();
    double attMinusInfluence = attitudeMinusInfDiffusion.getAttMinusInfluenceEstimation();
    IMResults::getInstance().setAttInfRatio(ratio);
    IMResults::getInstance().setAttitudeMinusInfluence(attMinusInfluence);

    // Result file
    string resultFile = constructResultFileName(graphFileName, budget, percentageTargets);
    IMResults::getInstance().writeToFile(resultFile);
    delete graph;
}

void executeHeuristic(cxxopts::ParseResult result) {
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = 100;

    Graph *graph = createGraphObject(result);
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        graph->setPropogationProbability(probability);
    }

    if(result.count("trivalency")>0) {
        bool trivalency = result["trivalency"].as<bool>();
        if(trivalency) {
            graph->generateEdgeProbabilitiesTrivalencyModel();
        }
    }
    loadResultsFileFrom(result);
    loadGraphSizeToResults(graph);

    string algorithm = result["algorithm"].as<std::string>();
    set<int> seedSet;
    if (algorithm.compare("topOutDegree")==0) {
        seedSet = graph->findTopKOutDegreeNodes(budget);
    } else {
        //Default to random algorithm
        seedSet = graph->findRandomNodes(budget);
    }
    int attitude = calculateIndependentAttitudeValue(graph, seedSet);
    IMResults::getInstance().setAttitude(attitude);
    string resultFile = constructResultFileName(graphFileName, budget, percentageTargets);
    IMResults::getInstance().writeToFile(resultFile);
    delete graph;
}

int main(int argc, char **argv) {
    cout << "Starting program\n";
    sfmt_t sfmt;
    srand(time(0));
    sfmt_init_gen_rand(&sfmt, rand());

    setupLogger();
    cout << "Setup logger \n";
    cxxopts::Options options("Targeted Influence Maximization", "Experiments and research.");
    options.add_options()
            ("algorithm", "Choose which algorithm to run", cxxopts::value<std::string>())
            ("graph", "Graph file name", cxxopts::value<std::string>())
            ("b,budget", "Budget size", cxxopts::value<int>())
            ("t,threshold", "NT threshold", cxxopts::value<int>())
            ("m,method", "TIM-TIM or SIM-SIM", cxxopts::value<int>())
            ("percentage", "Percentage of Targets", cxxopts::value<int>())
            ("n,ntfile", "Non Targets File name", cxxopts::value<std::string>())
            ("p,probability", "Propogation probability", cxxopts::value<double>())
            ("approximation", " Approximation Settings", cxxopts::value<int>())
            ("e,extend", "Extend the permutation")
            ("labelMethod", "Labelling Strategy", cxxopts::value<int>())
            ("decompositionFile", "Decomposition File", cxxopts::value<string>())
            ("nBuckets", "Number of Buckets", cxxopts::value<int>())
            ("model", "Cascade Model", cxxopts::value<string>())
            ("seedFile", "JSON File with Seed Set", cxxopts::value<string>())
            ("trivalency", "Trivalency Model")
            ("computeOutput", "Estimate the Output of the Seed Set after finding it")
            ("probabilitiesAssigned", "Edge probabilities are present in the input graph");
    auto result = options.parse(argc, argv);
    string algorithm = result["algorithm"].as<std::string>();
    if (algorithm.compare("greedy")==0) {
        executeGreedyAttitudeMaximization(result);
    } else if(algorithm.compare("attMinusInf")==0) {
        executeGreedyAttMinusInfMaximization(result);
    } else if(algorithm.compare("random")==0 || algorithm.compare("topOutDegree")==0) {
        executeHeuristic(result);
    } else if(algorithm.compare("estimate")==0){
        executeAttitudeEstimation(result);
    }
    disp_mem_usage("\n");
    return 0;
}
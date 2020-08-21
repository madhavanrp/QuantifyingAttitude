//
//  Graph.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/4/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "Graph.hpp"
#include <assert.h>
#include <sstream>
#include <iomanip>
using namespace std;
void Graph::readGraph(string fileName) {
    readGraph(fileName, 0.8);
}

void Graph::readGraph(string fileName, float percentage) {
    readGraph(fileName, percentage, LabelSettingUniform);
}

Graph::Graph() {
    this->standardProbability = false;
    this->diffusionModel = "IC";
    sfmt_init_gen_rand(&sfmt, rand());
}

void Graph::setDiffusionModel(string model) {
    this->diffusionModel = model;
}

void Graph::setPropogationProbability(float p) {
    this->propogationProbability = p;
    this->standardProbability = true;
    this->propogationProbabilityNumber = (float)1/p;
}

int Graph::getPropogationProbabilityNumber() {
    return this->propogationProbabilityNumber;
}

int Graph:: generateRandomNumber(int u, int v) {
    int randomNumberLimit;
    if(this->standardProbability) {
        randomNumberLimit = this->propogationProbabilityNumber;
    }
    else if(this->edgeProbabilitiesAssigned) {
        randomNumberLimit = this->edgeProbabilities[to_string(u) + "#" + to_string(v)];
    }
    else {
        randomNumberLimit = inDegree[v];
    }
    return sfmt_genrand_uint32(&sfmt) % randomNumberLimit;
}

double Graph::getWeightForLTModel(int u, int v) {
    return (double)1/(double)inDegree[v];
}

bool Graph::flipCoinOnEdge(int u, int v) {

    double randomDouble = sfmt_genrand_real1(&sfmt);

    if(this->standardProbability) {
        return randomDouble < this->propogationProbability;
    }
    else if(this->edgeProbabilitiesAssigned) {
        return randomDouble < this->edgeProbabilities[to_string(u) + "#" + to_string(v)];

    } else{
        return randomDouble < ( 1.0 / inDegree[v]);   // the default is p(u,v) = 1/ inDegree[v]
    }
}

vector<pair<int,int>>* Graph::getEdgeSet() {
    return &this->edgeSet;
}

void Graph::generateEdgeProbabilitiesTrivalencyModel() {
    double values[3] = {0.001, 0.01,0.1};
    int r;
    this->edgeProbabilitiesAssigned = true;
    for (int u = 0; u < n; ++u) {
        for (int v: graph[u]) {
            r = sfmt_genrand_uint32(&sfmt) % 3;
            edgeProbabilities[to_string(u) + "#" + to_string(v)] = values[r];
        }
    }
}

void Graph::readGraph(string fileName, float percentage, LabelSetting labelSetting) {
    this->graphName = fileName;
    this->percentageTargets = percentage;
    this->labelSetting = labelSetting;
    ifstream myFile("graphs/" + fileName);
    string s;
    if(!myFile.good()) {
        throw std::invalid_argument( "Graph file does not exist: " + fileName );
    }
    if(myFile.is_open()) {
        int beginningPosition = myFile.tellg();
        myFile >> n >> m;
        
        int from, to;
        double edgeProbability;
        bool probabilityAssigned = false;
        int maxDegree = 0;
        string line;
        myFile.seekg(beginningPosition);
        getline(myFile, line);
        int firstEdgePosition = (int)myFile.tellg();
        getline(myFile, line);
        istringstream iss(line);
        
        iss >> from >> to;
        probabilityAssigned = (iss >> edgeProbability)? true:false;
        this->edgeProbabilitiesAssigned = probabilityAssigned?true:false;
        
        for(int i =0;i<n;i++) {
            graph.push_back(vector<int>());
            visited.push_back(false);
            inDegree.push_back(0);
        }
        myFile.seekg(firstEdgePosition);
        while (myFile >> from >> to) {
            if (probabilityAssigned) {
                myFile >> edgeProbability;
                edgeProbabilities[to_string(from) + "#" + to_string(to)] = edgeProbability;
            }
            graph[from].push_back(to);
            inDegree[to] = inDegree[to]+1;
            if(inDegree[to] > maxDegree) {
                maxDegree = inDegree[to];
            }
            pair<int, int> edge;  //store an edge in the edge set
            edge.first = from;
            edge.second = to;
            this->edgeSet.push_back(edge);
        }
        assert(edgeSet.size() == m);
        myFile.close();
    }
    
    
    graphTranspose = constructTranspose(graph);
    visitMark = vector<int>(n);
    labels = vector<NodeLabel>(n);
    stringstream stream;
    stream << fixed << setprecision(2) << percentage;
    s = stream.str();
    if(percentage!=1) {
        string labelFileName = constructLabelFileName(this->graphName, percentage, labelSetting);
        cout << "\n Reading graph: " << fileName;
        cout << "\n Reading labels file name: " << labelFileName;
        readLabels(labelFileName);
    }
    else {
        for (int i=0; i<n; i++) {
            labels[i] = NodeLabelTarget;
        }
        this->numberOfNonTargets = 0;
        this->numberOfTargets = n;
    }
}

string Graph::constructLabelFileName(string graphName, float percentageTargets) {
    return constructLabelFileName(graphName, percentageTargets, LabelSettingUniform);
}

 string Graph::constructLabelFileName(string graphName, float percentageTargets, LabelSetting labelSetting) {
    stringstream stream;
    string folderName = "graphs/";
    stream << folderName;
    stream << graphName;
    stream << fixed << setprecision(2);
    stream << "_";
    stream << percentageTargets;
    stream << "_" << static_cast<int>(labelSetting) << "_labels.txt";
    return stream.str();
}

void Graph::readLabels(string fileName) {
    ifstream myFile(fileName);
    bool labelBool;
    if(!myFile.good()) {
        throw std::invalid_argument( "Label file does not exist: " + fileName );
    }
    string commentLine;
    getline(myFile, commentLine);
    if(myFile.is_open()) {
        int vertex;
        char label;
        while (myFile >> vertex >> label) {
            labelBool = (tolower(label)=='a');
            labels[vertex] = labelBool?NodeLabelTarget:NodeLabelNonTarget;
            if(!labelBool) {
                nonTargets.push_back(vertex);
            }
        }
        myFile.close();
    }
    this->numberOfTargets = this->getNumberOfVertices() - (int)nonTargets.size();
    this->numberOfNonTargets = (int)nonTargets.size();
}

void Graph::writeLabels(LabelSetting labelSetting) {
    writeLabels(labelSetting, "");
}

void Graph::writeLabels(LabelSetting labelSetting, string comment) {
    string s;
    stringstream stream;
    stream << fixed << setprecision(2) << this->percentageTargets;
    s = stream.str();
    string labelFileName = constructLabelFileName(this->graphName, this->percentageTargets, labelSetting);
    
    ofstream myfile;
    string fileName = labelFileName;
    myfile.open (fileName);
    string targetLabel = "A";
    string nonTargetLabel = "B";
    myfile << "# " << comment << "\n";
    for(int i=0; i<this->n; i++) {
        if(this->labels[i]==NodeLabelTarget) {
            myfile << i << " " << targetLabel << "\n";
        } else {
            myfile << i << " " << nonTargetLabel << "\n";
        }
        
    }
    myfile.close();
}

bool Graph::isTarget(int v) {
    return this->labels[v]==NodeLabelTarget;
}

bool Graph::isNonTarget(int v) {
    return this->labels[v]==NodeLabelNonTarget;
}

void Graph::setLabels(vector<NodeLabel> labels, float percentageTargets) {
    this->labels = labels;
    this->percentageTargets = percentageTargets;
}

int Graph::getNumberOfVertices() {
    return this->n;
}

int Graph::getNumberOfEdges() {
    return this->m;
}

int Graph::getNumberOfTargets() {
    return this->numberOfTargets;
}

int Graph::getNumberOfNonTargets() {
    return this->numberOfNonTargets;
}

vector<int>* Graph::getNonTargets() {
    return &this->nonTargets;
}

void Graph::generateRandomRRSets(int R) {
    this->rrSets = vector<vector<int>>();
    int totalSize = 0;
    clock_t begin = clock();
    for(int i=0;i<R;i++) {
        int randomVertex;
        randomVertex = sfmt_genrand_uint32(&sfmt) % n;
        while(this->labels[randomVertex]==NodeLabelNonTarget) {
            randomVertex = sfmt_genrand_uint32(&sfmt) % n;
        }
        generateRandomRRSet(randomVertex);
        totalSize+=rrSets[i].size();
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout <<"\n Generated " << R << " RR sets\n";
    cout << "Elapsed time " << elapsed_secs;
    cout<< " \n Time per RR Set is " << elapsed_secs/R;
    cout<< "\n Total Size is " << totalSize;
    cout<<"\n Average size is " << (float)totalSize/(float)R;
}

vector<vector<int>>* Graph::getRandomRRSets() {
    return &rrSets;
}

void Graph::clearRandomRRSets() {
    rrSets.clear();
    rrSets.swap(rrSets);
}

vector<vector<int>>* Graph::getGraph() {
    return &this->graph;
}

vector<vector<int>>* Graph::getGraphTranspose() {
    return &this->graphTranspose;
}

void Graph::generateRandomRRSet(int randomVertex) {
    q.clear();
    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    while(!q.empty()) {
        if (this->diffusionModel.compare("IC")==0) {
            int expand=q.front();
            q.pop_front();
            for(int j=0; j<(int)graphTranspose[expand].size(); j++){
                int v=graphTranspose[expand][j];
                if(!this->flipCoinOnEdge(v, expand))
                    continue;
                if(visited[v])
                    continue;
                if(!visited[v])
                {
                    visitMark[nVisitMark++]=v;
                    visited[v]=true;
                }
                q.push_back(v);
            }
        }
        else {
            // LT Model
            int u=q.front();
            q.pop_front();
            
            if(graphTranspose[u].size()==0)
                continue;
            double randomDouble = sfmt_genrand_res53(&sfmt);
            for(int i=0; i<(int)graphTranspose[u].size(); i++){
                int v = graphTranspose[u][i];
                randomDouble = randomDouble - this->getWeightForLTModel(v, u);
                if(randomDouble>0)
                    continue;
                
                if(visited[v])
                    break;
                visitMark[nVisitMark++]=v;
                visited[v]=true;
                q.push_back(v);
                
                break;
            }
        }
        
        
    }
    rrSets.push_back(vector<int>(visitMark.begin(), visitMark.begin()+nVisitMark));
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
        
    }
    
}

void Graph::generateRandomRRSetsWithoutVisitingNonTargets(int R) {
    this->rrSets = vector<vector<int>>();
    int totalSize = 0;
    clock_t begin = clock();
    while(rrSets.size()<R) {
        rrSets.push_back(vector<int>());
    }
    for(int i=0;i<R;i++) {
        int randomVertex;
        randomVertex = sfmt_genrand_uint32(&sfmt) % n;
        while(labels[randomVertex]==NodeLabelNonTarget) {
            randomVertex = sfmt_genrand_uint32(&sfmt) % n;
        }
        generateRandomRRSetWithoutVisitingNonTargets(randomVertex, i);
        totalSize+=rrSets[i].size();
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout <<"\n Generated " << R << " RR sets\n";
    cout << "Elapsed time " << elapsed_secs;
    cout<< " \n Time per RR Set is " << elapsed_secs/R;
    cout<< "\n Total Size is " << totalSize;
    cout<<"\n Average size is " << (float)totalSize/(float)R;
}

vector<int> Graph::generateRandomRRSetWithoutVisitingNonTargets(int randomVertex, int rrSetID) {
    q.clear();
    rrSets[rrSetID].push_back(randomVertex);
    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    while(!q.empty()) {
        int expand=q.front();
        q.pop_front();
        for(int j=0; j<(int)graphTranspose[expand].size(); j++){
            int v=graphTranspose[expand][j];
            if(labels[v]==NodeLabelNonTarget) continue;
            if(!this->flipCoinOnEdge(v, expand))
                continue;
            if(visited[v])
                continue;
            if(!visited[v])
            {
                visitMark[nVisitMark++]=v;
                visited[v]=true;
            }
            q.push_back(v);
            rrSets[rrSetID].push_back(v);
        }
    }
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
        
    }
    for(int v: rrSets[rrSetID]) {
        assert(labels[v]==NodeLabelTarget);
    }
    return rrSets[rrSetID];
    
}

vector<vector<int>> Graph::constructTranspose(vector<vector<int>> someGraph) {
    vector<vector<int>> transposedGraph = vector<vector<int>>();
    for(int i=0;i<someGraph.size();i++) {
        transposedGraph.push_back(vector<int>());
    }
    for(int i=0; i<someGraph.size();i++) {
        for(int v:someGraph[i]) {
            transposedGraph[v].push_back(i);
        }
    }
    return transposedGraph;
}

void Graph::assertTransposeIsCorrect() {
    assert(graph.size()==n);
    int edges = 0;
    
    for (int i=0; i< n; i++) {
        for (int j:graph[i]) {
            edges++;
        }
    }
    assert(edges==m);
    int edgeCount = 0;
    int reverseEdgeCount = 0;
    for (int i=0; i< n; i++) {
        int u = i;
        for (int j=0; j< graph[u].size(); j++) {
            edgeCount++;
            int v = graph[u][j];
            bool reverseEdgePresent = false;
            vector<int> reverseEdges = graphTranspose[v];
            for(int uPrime:reverseEdges) {
                if(uPrime==u) {
                    reverseEdgeCount++;
                    reverseEdgePresent = true;
                }
            }
            assert(reverseEdgePresent);
        }
        
    }
    assert(edgeCount==reverseEdgeCount);
    assert(edgeCount==m);
    
}

set<int> Graph::findTopKOutDegreeNodes(int k) {
    set<int> nodes;
    priority_queue<pair<int, int>, vector<pair<int, int>>, PairComparator> queue;
    for (int i = 0; i < this->n; ++i) {
        queue.push(make_pair(i, this->graph[i].size()));
    }
    while (nodes.size()<k) {
        nodes.insert(queue.top().first);
        queue.pop();
    }
    return nodes;

}

set<int> Graph::findRandomNodes(int k) {
    set<int> nodes;
    int randomNode;
    while(nodes.size()<k) {
        do {
            randomNode = sfmt_genrand_uint32(&sfmt) % n;
        } while(nodes.find(randomNode)!=nodes.end());

        nodes.insert(randomNode);
    }
    assert(nodes.size()==k);
    cout << "\n K is" << k;
    cout << "\n Nodes size is " << nodes.size() << flush;
    return nodes;
}
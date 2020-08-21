//
// Created by Madhavan R.P on 2019-05-06.
//

#ifndef INFLUENCEMAXIMIZATION_IMSEEDSET_HPP
#define INFLUENCEMAXIMIZATION_IMSEEDSET_HPP


#include <vector>
#include <set>
using namespace std;
class IMSeedSet {
    vector<int> seedVector;
    set<int> seedSet;

public:
    int targets;
    int nonTargets;
    set<int> getSeedSet() {
        return this->seedSet;
    }
    int getTargets() {
        return this->targets;
    }
    int getNonTargets() {
        return this->nonTargets;
    }

    IMSeedSet() {
        this->targets = 0;
        this->nonTargets = 0;
    }

    void setSeedSet(set<int> seedSet) {
        this->seedSet = seedSet;
    }

    void setSeedVector(vector<int> seedVector) {
        this->seedVector = seedVector;
    }

    vector<int> getSeedVector() {
        return this->seedVector;
    }

    void setTargets(int targets) {
        this->targets = targets;
    }

    void setNonTargets(int nonTargets) {
        this->nonTargets = nonTargets;
    }

    void addSeed(int nodeID) {
        this->seedVector.push_back(nodeID);
        this->seedSet.insert(nodeID);
    }
};


#endif //INFLUENCEMAXIMIZATION_IMSEEDSET_HPP

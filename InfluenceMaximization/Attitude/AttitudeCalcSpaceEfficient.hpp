//
// Created by Madhavan Rajagopal Padmanabhan on 2019-08-08.
//

#ifndef INFLUENCEMAXIMIZATION_ATTITUDECALCSPACEEFFICIENT_HPP
#define INFLUENCEMAXIMIZATION_ATTITUDECALCSPACEEFFICIENT_HPP


#include <InfluenceMaximization/Graph/Graph.hpp>
#include <InfluenceMaximization/RISCalculation/RISCalcSpaceEfficient.hpp>
#include <set>
#include <cmath>

class AttitudeCalcSpaceEfficient: public RISCalcSpaceEfficient {
public:
    using RISCalcSpaceEfficient::RISCalcSpaceEfficient;
    virtual double calculateNumberOfRRSets();
    virtual double estimate(std::set<int> seedSet);
};


#endif //INFLUENCEMAXIMIZATION_ATTITUDECALCSPACEEFFICIENT_HPP

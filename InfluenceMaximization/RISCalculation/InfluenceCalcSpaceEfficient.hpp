//
// Created by Madhavan Rajagopal Padmanabhan on 2019-08-14.
//

#ifndef INFLUENCEMAXIMIZATION_INFLUENCECALCSPACEEFFICIENT_HPP
#define INFLUENCEMAXIMIZATION_INFLUENCECALCSPACEEFFICIENT_HPP


#include "RISCalcSpaceEfficient.hpp"

class InfluenceCalcSpaceEfficient: public RISCalcSpaceEfficient {
public:
    using RISCalcSpaceEfficient::RISCalcSpaceEfficient;
    virtual double calculateNumberOfRRSets();
    virtual double estimate(std::set<int> seedSet);
};


#endif //INFLUENCEMAXIMIZATION_INFLUENCECALCSPACEEFFICIENT_HPP

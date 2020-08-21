//
// Created by Madhavan Rajagopal Padmanabhan on 2020-01-04.
//

#ifndef INFLUENCEMAXIMIZATION_ATTITUDEMINUSINFDIFFUSION_HPP
#define INFLUENCEMAXIMIZATION_ATTITUDEMINUSINFDIFFUSION_HPP

#include <InfluenceMaximization/DiffusionAlgorithms/DiffusionBase.hpp>

class AttitudeMinusInfDiffusion: public DiffusionBase {
protected:
    double attitudeEstimation;
    double influenceEstimation;
    double ratioEstimation;
    double attMinusInfluenceEstimation;
public:
    double getAttitudeEstimation();
    double getInfluenceEstimation();
    double getRatioEstimation();
    double getAttMinusInfluenceEstimation();

    using DiffusionBase::DiffusionBase;
    double performOperation();
    double performDiffusion(set<int> seedSet);
};


#endif //INFLUENCEMAXIMIZATION_ATTITUDEMINUSINFDIFFUSION_HPP

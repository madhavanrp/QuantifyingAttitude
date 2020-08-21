cmake_minimum_required(VERSION 3.14)
project(InfluenceMaximization)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -O3")

include_directories(.)
include_directories(InfluenceMaximization)
include_directories(InfluenceMaximization/Graph)
include_directories(InfluenceMaximization/IMResults)
include_directories(InfluenceMaximization/TIM-Models)
include_directories(InfluenceMaximization/Models)
include_directories(SFMT)

file(GLOB Graph_SOURCES
        "InfluenceMaximization/Graph/*.hpp"
        "InfluenceMaximization/Graph/*.cpp")


file(GLOB SFMT_SOURCES
        "SFMT/SFMT.h"
        "SFMT/SFMT.c")


set(Common_SOURCES
        ${SFMT_SOURCES}
        InfluenceMaximization/IMResults/IMResults.h
        InfluenceMaximization/log.h
        InfluenceMaximization/memoryusage.h
        InfluenceMaximization/json.hpp
        InfluenceMaximization/DiffusionAlgorithms/Diffusion.h
        InfluenceMaximization/DiffusionAlgorithms/Diffusion.cpp
        InfluenceMaximization/TIMInfluenceCalculator.hpp
        InfluenceMaximization/TIMInfluenceCalculator.cpp
        InfluenceMaximization/TIM-Models/TIMCoverage.hpp
        InfluenceMaximization/TIM-Models/TIMCoverage.cpp
        InfluenceMaximization/Models/IMSeedSet.hpp
        cxxopts.hpp
        main_utils.h InfluenceMaximization/RISCalculation/RISCalcSpaceEfficient.cpp InfluenceMaximization/RISCalculation/RISCalcSpaceEfficient.hpp InfluenceMaximization/RISCalculation/InfluenceCalcSpaceEfficient.cpp InfluenceMaximization/RISCalculation/InfluenceCalcSpaceEfficient.hpp InfluenceMaximization/DiffusionAlgorithms/AttitudeMinusInfDiffusion.cpp InfluenceMaximization/DiffusionAlgorithms/AttitudeMinusInfDiffusion.hpp InfluenceMaximization/DiffusionAlgorithms/DiffusionBase.cpp InfluenceMaximization/DiffusionAlgorithms/DiffusionBase.hpp)


add_executable(attitude
        ${Common_SOURCES}
        ${Graph_SOURCES}
        attitude_main.cpp InfluenceMaximization/Attitude/AttitudeCalculator.cpp InfluenceMaximization/Attitude/AttitudeCalculator.hpp InfluenceMaximization/Attitude/AttitudeMaximizer.cpp InfluenceMaximization/Attitude/AttitudeMaximizer.hpp InfluenceMaximization/Attitude/AttitudeCalcSpaceEfficient.cpp InfluenceMaximization/Attitude/AttitudeCalcSpaceEfficient.hpp InfluenceMaximization/Attitude/AttitudeMinusInfMaximizer.cpp InfluenceMaximization/Attitude/AttitudeMinusInfMaximizer.hpp InfluenceMaximization/TIM-Models/RISRRGraph.cpp InfluenceMaximization/TIM-Models/RISRRGraph.hpp)

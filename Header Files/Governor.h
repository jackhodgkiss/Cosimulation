#ifndef COSIMULATION_GOVERNOR_H
#define COSIMULATION_GOVERNOR_H

#include <map>
#include <memory>
#include <string>
#include <random>
#include <bitset>
#include "Vehicle.h"

/**
 * This class will govern all vehicles within the simulation. This class will instruct vehicles at what time they are
 * to desire to change lane and which lane to change to. It is upon the vehicle and their application to actual
 * implement this change. Also each vehicle will step within this class.
 */
class Governor
{
    std::map<std::string, std::shared_ptr<Vehicle>> vehicles;
    std::shared_ptr<TraCIClient> client;
    std::bitset<5> selection_lanes;
    std::mt19937 random_generator;
    std::uniform_real_distribution<double> distribution = std::uniform_real_distribution<double>(0, 1);
    double selection_probability;
    int selection_interval;
    void SelectVehicles();
public:
    Governor(std::map<std::string, std::shared_ptr<Vehicle>> Vehicles, std::shared_ptr<TraCIClient> Client,
             std::bitset<5> Selection_Lanes, double Selection_Probability, int Selection_Interval, int Seed);
    Governor() = default;
    ~Governor() = default;
    void Step();
    void ScheduleSelection();
};

#endif
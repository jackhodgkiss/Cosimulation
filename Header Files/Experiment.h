#ifndef COSIMULATION_EXPERIMENT_H
#define COSIMULATION_EXPERIMENT_H

#include <map>
#include <memory>
#include <string>
#include "Vehicle.h"
#include "Governor.h"
#include "TraCIClient.h"
#include "VehicleFactory.h"
#include "Configuration.h"

/**
 * Experiment class will act as the entry point into the simulation. It will facilitate the initialisation,
 * configuration, running and teardown of the simulation.
 */
class Experiment
{
    Governor governor;
    VehicleFactory factory;
    Configuration configuration;
    std::shared_ptr<TraCIClient> client = std::make_shared<TraCIClient>();
    std::map<std::string, std::shared_ptr<Vehicle>> vehicles;
    void Initialise();
    void Step();
    void Run();
public:
    Experiment(int argc, char** argv);
    ~Experiment() = default;
};

#endif
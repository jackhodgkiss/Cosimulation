#include "../Header Files/Experiment.h"
#include <vector>
#include <ns3/core-module.h>
#include <ns3/animation-interface.h>
#include "../Header Files/ILACHApplication.h"
#include "../Header Files/VehicleApplication.h"
#include "../Header Files/ILACHPlusApplication.h"

using namespace ns3;

/**
 * Construct a new experiment.
 */
Experiment::Experiment(int argc, char** argv)
        : configuration(Configuration(argc, argv))
{
    this->Initialise();
    this->Run();
}

/**
 * Initialisation code goes here.
 */
void Experiment::Initialise()
{
    this->client->connect(this->configuration.Remote_Address, this->configuration.Remote_Port);
    while(this->client->simulation.getMinExpectedNumber() > 0)
    {
        std::vector<std::string> id_list = this->client->vehicle.getIDList();
        for(const auto& id : id_list)
        {
            if(this->vehicles.find(id) == this->vehicles.end())
            {
                ns3::Ptr<VehicleApplication> vehicle_application;
                if(this->configuration.Use_Enhanced)
                {
                    vehicle_application = Create<ILACHPlusApplication>();
                }
                else
                {
                    vehicle_application = Create<ILACHApplication>();
                }
                std::shared_ptr<Vehicle> vehicle = this->factory.CreateVehicle(id);
                vehicle_application->Install(vehicle, this->client);
                this->vehicles.insert(std::pair<std::string, std::shared_ptr<Vehicle>>(id, vehicle));
            }
        }
        this->client->simulationStep();
    }
    std::vector<std::string> reload_arguments = {"-c", this->configuration.SUMO_URL, "--remote-port",
                                                 std::to_string(this->configuration.Remote_Port),
                                                 "--step-length", std::to_string(this->configuration.Step_Length)};
    if(!this->configuration.Lane_Change_Output.empty())
    {
        reload_arguments.push_back("--lanechange-output");
        reload_arguments.push_back(this->configuration.Lane_Change_Output);
    }
    if(!this->configuration.Trip_Info_Output.empty())
    {
        reload_arguments.push_back("--tripinfo-output");
        reload_arguments.push_back(this->configuration.Trip_Info_Output);
    }
    this->client->load(reload_arguments);
    for(int i = 0; i < 4; i++)
    {
        std::string lane_id = "gneE0_";
        lane_id.append(std::to_string(i));
        this->client->ChangeLaneSpeedLimit(lane_id, this->configuration.Lane_Speed_Limits.at(i));
    }
    this->governor = Governor(this->vehicles, this->client,
                              this->configuration.Selection_Lanes, this->configuration.Selection_Probability,
                              this->configuration.Selection_Interval, this->configuration.Seed);
    this->governor.ScheduleSelection();
}

/**
 * Each step between the two simulations is handled here.
 */
void Experiment::Step()
{
    if(this->client->simulation.getMinExpectedNumber() > 0)
    {
        this->governor.Step();
        Simulator::Schedule(MilliSeconds((uint64_t)this->configuration.Step_Length * 1000), &Experiment::Step, this);
    }
}

/**
 * Start the simulation.
 */
void Experiment::Run()
{
    if(this->configuration.Animation_URL.empty())
    {
        Simulator::Schedule(MilliSeconds(0), &Experiment::Step, this);
        Simulator::Run();
        Simulator::Destroy();
        this->client->close();
    }
    else
    {
        AnimationInterface animation = AnimationInterface(this->configuration.Animation_URL);
        Simulator::Schedule(MilliSeconds(0), &Experiment::Step, this);
        Simulator::Run();
        Simulator::Destroy();
        this->client->close();
    }
}
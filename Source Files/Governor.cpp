#include "../Header Files/Governor.h"
#include <algorithm>
#include <ns3/nstime.h>
#include <ns3/simulator.h>
#include <ns3/mobility-model.h>

/**
 * Construct a new governor that is configured to oversee all vehicles within the simulation.
 * @param Vehicles Collection of vehicles that populate the simulation space.
 * @param Client TraCIClient will valid established connection to the TraCI server.
 * @param Selection_Lanes Lanes which will be used to select vehicle to change lane from. Other lanes ignored.
 * @param Selection_Probability The probability that a vehicle maybe selected.
 * @param Selection_Interval The length of time between selection processes.
 * @param Seed The random generator will be seeded with this value.
 */
Governor::Governor(std::map<std::string, std::shared_ptr<Vehicle>> Vehicles, std::shared_ptr<TraCIClient> Client,
                   std::bitset<5> Selection_Lanes, double Selection_Probability, int Selection_Interval, int Seed)
{
    this->vehicles = Vehicles;
    this->client = Client;
    this->selection_lanes = Selection_Lanes;
    this->selection_probability = Selection_Probability;
    this->selection_interval = Selection_Interval;
    this->random_generator = std::mt19937(Seed);
}

/**
 * Step through each of the vehicles in the simulation.
 */
void Governor::Step()
{
    std::vector<std::string> id_list = this->client->vehicle.getIDList();
    for(const auto& id : id_list)
    {
        if(this->vehicles.find(id) != this->vehicles.end())
        {
            ns3::Ptr<ns3::MobilityModel> mobility = this->vehicles.at(id)->GetNode()->GetObject<ns3::MobilityModel>();
            if(mobility->GetPosition().z == 10000 && mobility->GetPosition().x == 0)
            {
                this->client->SetLaneChangeMode(id, 256);
            }
            this->vehicles.at(id)->Step(this->client);
        }
    }
    for(const auto& pair : this->vehicles)
    {
        std::string id = pair.first;
        ns3::Ptr<ns3::MobilityModel> mobility = this->vehicles.at(id)->GetNode()->GetObject<ns3::MobilityModel>();
        ns3::Vector position = mobility->GetPosition();
        if(std::find(id_list.begin(), id_list.end(), id) == id_list.end() && position.z != 10000)
        {
            position.z = 10000;
            mobility->SetPosition(position);
        }
    }
    this->client->simulationStep();
}

/**
 * Select vehicles currently active within the simulation to change lane.
 */
void Governor::SelectVehicles()
{
    std::vector<std::string> id_list = this->client->vehicle.getIDList();
    for(const auto& id : id_list)
    {
        if(this->vehicles.find(id) != this->vehicles.end())
        {
            std::shared_ptr<Vehicle> vehicle = this->vehicles.at(id);
            ns3::Ptr<ns3::MobilityModel> mobility = vehicle->GetNode()->GetObject<ns3::MobilityModel>();
            ns3::Vector position = mobility->GetPosition();
            if(position.z != 10000 && this->selection_lanes.test((size_t)vehicle->GetAttributes()->Lane_Index))
            {
                if(!vehicle->HasTarget() && this->distribution(random_generator) < this->selection_probability)
                {
                    int target_delta;
                    int current_lane = vehicle->GetAttributes()->Lane_Index;
                    if(current_lane == 4 || current_lane == 0)
                    {
                        if(current_lane == 4)
                        {
                            target_delta = -1;
                        }
                        else
                        {
                            target_delta = 1;
                        }
                    }
                    else
                    {
                        target_delta = distribution(random_generator) <= 0.5 ? 1 : -1;
                    }
                    vehicle->SetTarget(current_lane + target_delta);
                }
            }
        }
    }
    if(this->client->simulation.getMinExpectedNumber() > 0)
    {
        this->ScheduleSelection();
    }
}

/**
 * Schedule the selection function to be called when required.
 */
void Governor::ScheduleSelection()
{
    ns3::Simulator::Schedule(ns3::Seconds(this->selection_interval), &Governor::SelectVehicles, this);
}

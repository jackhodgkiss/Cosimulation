#include "../Header Files/Vehicle.h"
#include <algorithm>
#include <ns3/ipv4.h>
#include <ns3/core-module.h>
#include <ns3/waypoint-mobility-model.h>
#include "../Header Files/VehicleApplication.h"

using namespace ns3;

/**
 * Construct a new vehicle with the assigned network facilities and a unique identifier.
 * @param Vehicle_Node NS-3 network node.
 * @param Vehicle_Devices Container of network devices.
 * @param Vehicle_Attributes Attributes associated with this vehicle.
 * @param Vehicle_ID Unique identifier.
 */
Vehicle::Vehicle(Ptr<Node> Vehicle_Node, NetDeviceContainer Vehicle_Devices,
                 std::shared_ptr<VehicleAttributes> Vehicle_Attributes, std::string ID)
{
    this->vehicle_node = Vehicle_Node;
    this->vehicle_devices = Vehicle_Devices;
    this->vehicle_attributes = Vehicle_Attributes;
    this->vehicle_id = ID;
}

/**
 * Update the vehicle at the current step within the simulation. Collect and assign attributes and make any required
 * decisions before progressing into the next time step.
 * @param Client TraCIClient that enable communication to SUMO.
 */
void Vehicle::Step(std::shared_ptr<TraCIClient> Client)
{
    Client->simulation.subscribe(CMD_SUBSCRIBE_VEHICLE_VARIABLE, this->GetID(),
                                 Client->simulation.getCurrentTime(), Client->simulation.getCurrentTime() + 1,
                                 this->GetAttributes()->Attribute_Names);
    this->GetAttributes()->Update(Client->simulation.getSubscriptionResults(this->GetID()));
    Ptr<WaypointMobilityModel> mobility = this->GetNode()->GetObject<WaypointMobilityModel>();
    Vector position = Vector(this->GetAttributes()->Position.x, this->GetAttributes()->Position.y, 0);
    mobility->AddWaypoint(Waypoint(Simulator::Now(), position));
    int current_lane = this->GetAttributes()->Lane_Index;
    if(this->HasTarget())
    {
        // Vehicle has a target currently set. Lets ensure that we haven't already reached it.
        if(this->target_lane != current_lane)
        {
            // We are still moving towards are target lane. Lets send a request to change lane if we haven't already.
            if(this->previous_lane != current_lane)
            {
                Ptr<VehicleApplication> vehicle_application =
                        this->GetNode()->GetApplication(0)->GetObject<VehicleApplication>();
                // Need to determine if the target lane is above or below the current lane.
                if(this->target_lane > current_lane)
                {
                    vehicle_application->ChangeLane(current_lane + 1);
                }
                else
                {
                    vehicle_application->ChangeLane(current_lane - 1);
                }
                this->previous_lane = current_lane; // Set previous lane so we don't call upon the application again.
            }
        }
        else
        {
            this->SetTarget(-1); // We have reached our target so lets reset and wait for another target from Governor.
        }
    }
}

void Vehicle::VerifyLaneChange(int Lane_Index)
{
    Ptr<VehicleApplication> vehicle_application =
            this->GetNode()->GetApplication(0)->GetObject<VehicleApplication>();
    if(this->GetAttributes()->Lane_Index != Lane_Index)
        vehicle_application->ChangeLane(Lane_Index);
}

void Vehicle::RecommendLaneChange(bool Recommendation, int Lane_Index, std::shared_ptr<TraCIClient> Client)
{
    std::vector<std::string> id_list = Client->vehicle.getIDList();
    if(std::find(id_list.begin(), id_list.end(), this->GetID()) != id_list.end())
    {
        if(Recommendation)
        {
            Client->ChangeLane(this->GetID(), Lane_Index, 0);
            Simulator::Schedule(Seconds(10), &Vehicle::VerifyLaneChange, this, Lane_Index);
        }
        else
        {
            Ptr<VehicleApplication> vehicle_application =
                    this->GetNode()->GetApplication(0)->GetObject<VehicleApplication>();
            Simulator::Schedule(Seconds(10), &VehicleApplication::ChangeLane, vehicle_application, Lane_Index);
        }
    }
}

/**
 * Get the network node associated with this vehicle.
 * @return Network node associated with this vehicle.
 */
Ptr<Node> Vehicle::GetNode()
{
    return this->vehicle_node;
}

/**
 * Get the container of network devices associated with this vehicle.
 * @return Container of network devices associated with this vehicle.
 */
NetDeviceContainer& Vehicle::GetDevices()
{
    return this->vehicle_devices;
}

/**
 * Get the attributes associated with this vehicle.
 * @return Attributes associated with this vehicle.
 */
std::shared_ptr<VehicleAttributes> Vehicle::GetAttributes()
{
    return this->vehicle_attributes;
}

/**
 * Get the unique identifier assigned to this vehicle.
 * @return Unique identifier assigned to this vehicle.
 */
std::string Vehicle::GetID()
{
    return this->vehicle_id;
}

/**
 * Get the IP address assigned to the network devices attached to this vehicle.
 * 1.  Obtain the IPV4 address assigned to the network node on board this vehicle.
 * 2.  Serialise the IP address to a buffer.
 * 3.  For each of the octets within the IP address:
 * 3.1 Convert the octet from a uint8_t => string
 * 3.2 Append the converted octet to the string.
 * 3.3 If the octet is not that last octet that append a '.'
 * 4.  Return the result.
 * @return IP address assigned to the network devices attached to this vehicle.
 */
std::string Vehicle::GetIPAddress()
{
    std::string result;
    uint8_t buffer[4];
    Ipv4Address address = this->GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    address.Serialize(buffer);
    for(auto i = 0; i < 4; i++)
    {
        result.append(std::to_string(buffer[i]));
        if(i != 3) result.append(".");
    }
    return result;
}

void Vehicle::SetTarget(int Target_Lane)
{
    this->target_lane = Target_Lane;
}

bool Vehicle::HasTarget()
{
    return this->target_lane != -1;
}

/**
 * Get the current state of this vehicle as a string.
 * @return Current state of this vehicle as a string.
 */
std::string Vehicle::ToString()
{
    char buffer[512];
    std::sprintf(buffer, "ID: %s, IP Address: %s %s", this->GetID().c_str(), this->GetIPAddress().c_str(),
                 this->GetAttributes()->ToString().c_str());
    return std::string(buffer);
}

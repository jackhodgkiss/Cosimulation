#include "../Header Files/ILACHPlusApplication.h"
#include <ns3/simulator.h>

using namespace ns3;

void ILACHPlusApplication::RunAlgorithm(int Lane_Index)
{
    std::pair<Address, VehicleAttributes> partner;
    bool has_partner = this->GetPartner(partner);
    if(has_partner)
    {
        std::pair<Address, VehicleAttributes> leader;
        std::pair<Address, VehicleAttributes> follower;
        bool has_leader = this->GetLeader(leader);
        bool has_follower = this->GetFollower(follower);
        VehicleAttributes vi_attributes = *this->GetVehicleAttributes();
        VehicleAttributes vj_attributes = partner.second;
        libsumo::TraCIPosition position = vi_attributes.Position;
        libsumo::TraCIPosition partner_position = vj_attributes.Position;
        double n_loc = position.x + 50.0;
        double d_switch = n_loc - position.x;
        double t_switch = d_switch / vi_attributes.Speed;
        double d_gab = fabs(n_loc - partner_position.x);
        double t_gab = 0;
        double t_switch_2 = 0;
        // Does the vehicle have the speed required to reach it intended location in the target lane at current speed.
        if((d_gab / t_switch) < vj_attributes.Speed)
        {
            // Vehicle can be recommended to change lane.
            if(this->IsPresent())
                this->GetVehicle()->RecommendLaneChange(true, Lane_Index, this->GetClient());
        }
        else
        {
            libsumo::TraCIPosition leader_position = leader.second.Position;
            libsumo::TraCIPosition follower_position = follower.second.Position;
            // Lets see if the vehicle has room to move. If not lets ask our partner to move for us.
            if(!has_follower || (follower_position.x != 0 &&
                                 fabs(follower_position.x - position.x)) > (4 * vi_attributes.Length)
                    || !has_leader || (leader_position.x != 0 &&
                                         fabs(leader_position.x - position.x)) > (4 * vi_attributes.Length))
            {
                // Vehicle may only change lane if it is capable of accelerating to create adequate gap.
                t_gab = d_gab / vj_attributes.Speed;
                t_switch_2 = sqrt(d_switch / vi_attributes.Acceleration);
                if(t_switch_2 < t_gab)
                {
                    // Vehicle may change lane as it is capable of accelerating to create gap.
                    if(this->IsPresent())
                    {
                        this->GetClient()->vehicle.slowDown(this->GetVehicleID(), vi_attributes.Speed + 6.00, 8000);
                        this->GetVehicle()->RecommendLaneChange(true, Lane_Index, this->GetClient());
                    }
                }
                else
                {
                    // Vehicle should wait until the partner has passed and retry. No recommendation can be given.
                    if(this->IsPresent())
                    {
                        this->GetClient()->vehicle.slowDown(this->GetVehicleID(),
                                                            this->GetVehicleAttributes()->Speed / (2.0 / 3.0), 8000);
                        this->GetVehicle()->RecommendLaneChange(false, Lane_Index, this->GetClient());
                    }
                }
            }
            else
            {
                Simulator::Schedule(this->GetTransmissionDelay(), &ILACHPlusApplication::Send, this,
                                    Command, std::string("Slow_Down"), partner.first);
                Simulator::Schedule(Seconds(4), &Vehicle::RecommendLaneChange, this->GetVehicle().get(),
                                    true, Lane_Index, this->GetClient());
            }
        }
    }
    else
    {
        if(this->IsPresent())
            this->GetVehicle()->RecommendLaneChange(true, Lane_Index, this->GetClient());
    }
}

void ILACHPlusApplication::Receive(Ptr<Socket> Socket)
{
    Ptr<Packet> packet;
    Address from;
    while(packet = Socket->RecvFrom(from))
    {
        std::string content;
        Context action = this->Read(packet, content);
        if(action == Get)
        {
            int target_lane = std::stoi(content.substr(0, 1));
            int current_lane = std::stoi(content.substr(2, 1));
            if(this->GetVehicleAttributes()->Lane_Index == target_lane ||
                                                          this->GetVehicleAttributes()->Lane_Index == current_lane) {
                Simulator::Schedule(this->GetTransmissionDelay(), &ILACHPlusApplication::Send, this,
                                    Response, VehicleAttributes::Serialise(*this->GetVehicleAttributes()), from);
            }
        }
        else if(action == Response)
        {
            std::pair<Address, VehicleAttributes> pair(from, VehicleAttributes::Deserialise(content));
            this->GetResponses().insert(pair);
        }
        else if(action == Command)
        {
            if(content.compare(0, 9, "Slow_Down") == 0)
            {
                if(this->IsPresent())
                {
                    this->GetClient()->vehicle.slowDown(this->GetVehicleID(),
                                                        this->GetVehicleAttributes()->Speed / (2.0 / 3.0), 8000);
                }
            }
        }
    }
}

void ILACHPlusApplication::ChangeLane(int Lane_Index)
{
    this->GetResponses().clear();
    if(this->IsPresent())
    {
        char buffer[4];
        std::sprintf(buffer, "%d/%d", Lane_Index, this->GetVehicleAttributes()->Lane_Index);
        Simulator::Schedule(this->GetTransmissionDelay(), &ILACHPlusApplication::Send, this,
                            Get, std::string(buffer), Ipv4Address::GetZero());
        Simulator::Schedule(MilliSeconds(100), &ILACHPlusApplication::RunAlgorithm, this, Lane_Index);
    }
}

bool ILACHPlusApplication::GetLeader(std::pair<Address, VehicleAttributes>& Leader)
{
    bool result = false;
    if(!this->GetResponses().empty())
    {
        double position_x = this->GetVehicleAttributes()->Position.x;
        std::map<Address, VehicleAttributes> in_same_lane_and_in_front;
        for(const auto& pair : this->GetResponses())
        {
            if(pair.second.Lane_Index == this->GetVehicleAttributes()->Lane_Index &&
               pair.second.Position.x > position_x)
            {
                in_same_lane_and_in_front.insert(std::pair<Address, VehicleAttributes>(pair.first, pair.second));
            }
        }
        if(!in_same_lane_and_in_front.empty())
        {
            Leader.first = in_same_lane_and_in_front.begin()->first;
            Leader.second = in_same_lane_and_in_front.begin()->second;
            for(const auto& pair : in_same_lane_and_in_front)
            {
                if(abs((int)(pair.second.Position.x - position_x)) <=
                   abs((int)(Leader.second.Position.x - position_x)))
                {
                    Leader.first = pair.first;
                    Leader.second = pair.second;
                }
            }
            result = true;
        }
    }
    return result;
}

bool ILACHPlusApplication::GetFollower(std::pair<Address, VehicleAttributes>& Follower)
{
    bool result = false;
    if(!this->GetResponses().empty());
    {
        double position_x = this->GetVehicleAttributes()->Position.x;
        std::map<Address, VehicleAttributes> in_same_lane_and_behind;
        for(const auto& pair : this->GetResponses())
        {
            if(pair.second.Lane_Index == this->GetVehicleAttributes()->Lane_Index &&
               pair.second.Position.x < position_x)
            {
                in_same_lane_and_behind.insert(std::pair<Address, VehicleAttributes>(pair.first, pair.second));
            }
        }
        if(!in_same_lane_and_behind.empty())
        {
            Follower.first = in_same_lane_and_behind.begin()->first;
            Follower.second = in_same_lane_and_behind.begin()->second;
            for(const auto& pair : in_same_lane_and_behind)
            {
                if(abs((int)(pair.second.Position.x - position_x)) <=
                   abs((int)(Follower.second.Position.x - position_x)))
                {
                    Follower.first = pair.first;
                    Follower.second = pair.second;
                }
            }
            result = true;
        }
    }
    return result;
}
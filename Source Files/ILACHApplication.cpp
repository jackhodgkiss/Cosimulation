#include "../Header Files/ILACHApplication.h"
#include <ns3/simulator.h>

using namespace ns3;

void ILACHApplication::RunAlgorithm(int Lane_Index)
{
    std::pair<Address, VehicleAttributes> partner;
    bool has_partner = this->GetPartner(partner);
    if(has_partner)
    {
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
    }
    else
    {
        // Since there is no partner in the target lane the vehicle may change when ready.
        if(this->IsPresent())
            this->GetVehicle()->RecommendLaneChange(true, Lane_Index, this->GetClient());
    }
}

void ILACHApplication::Receive(ns3::Ptr<ns3::Socket> Socket)
{
    Ptr<Packet> packet;
    Address from;
    while(packet = Socket->RecvFrom(from))
    {
        std::string content;
        Context action = this->Read(packet, content);
        if(action == Get)
        {
            if(this->GetVehicleAttributes()->Lane_Index == std::stoi(content)) {
                Simulator::Schedule(this->GetTransmissionDelay(), &ILACHApplication::Send, this,
                                    Response, VehicleAttributes::Serialise(*this->GetVehicleAttributes()), from);
            }
        }
        else if(action == Response)
        {
            std::pair<Address, VehicleAttributes> pair(from, VehicleAttributes::Deserialise(content));
            this->GetResponses().insert(pair);
        }
    }
}

void ILACHApplication::ChangeLane(int Lane_Index)
{
    if(this->IsPresent())
    {
        this->GetResponses().clear();
        Simulator::Schedule(this->GetTransmissionDelay(), &ILACHApplication::Send, this,
                            Get, std::to_string(Lane_Index), Ipv4Address::GetZero());
        Simulator::Schedule(MilliSeconds(100), &ILACHApplication::RunAlgorithm, this, Lane_Index);
    }
}

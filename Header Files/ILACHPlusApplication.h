#ifndef COSIMULATION_ILACHPLUSAPPLICATION_H
#define COSIMULATION_ILACHPLUSAPPLICATION_H

#include "VehicleApplication.h"

/**
 * This class is responsible for implementing the ILACHPlus protocol within this simulation. It shall utilise the
 * VehicleApplication as foundation.
 */
class ILACHPlusApplication : public VehicleApplication
{
private:
    bool GetLeader(std::pair<ns3::Address, VehicleAttributes>& Leader);
    bool GetFollower(std::pair<ns3::Address, VehicleAttributes>& Follower);
protected:
    virtual void RunAlgorithm(int Lane_Index);
    virtual void Receive(ns3::Ptr<ns3::Socket> Socket);
public:
    virtual void ChangeLane(int Lane_Index);
};

#endif
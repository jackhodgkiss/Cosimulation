#ifndef COSIMULATION_ILACHAPPLICATION_H
#define COSIMULATION_ILACHAPPLICATION_H

#include "VehicleApplication.h"

/**
 * This class is responsible for implementing the ILACH protocol within this simulation. It shall utilise the
 * VehicleApplication as foundation.
 */
class ILACHApplication : public VehicleApplication
{
protected:
    virtual void RunAlgorithm(int Lane_Index);
    virtual void Receive(ns3::Ptr<ns3::Socket> Socket);
public:
    virtual void ChangeLane(int Lane_Index);
};

#endif
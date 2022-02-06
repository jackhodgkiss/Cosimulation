#ifndef COSIMULATION_VEHICLE_H
#define COSIMULATION_VEHICLE_H

#include <memory>
#include <string>
#include <ns3/node.h>
#include "TraCIClient.h"
#include "VehicleAttributes.h"
#include <ns3/net-device-container.h>

/**
 * This class is responsible for acting as a common interface between the two simulators used within this application.
 * NS-3 is represented with the use of the network node and network devices each configured appropriately to conform to
 * IEEE 802.11p standards. SUMO is represented with the use of the unique identifier that can be used to query SUMO via
 * the TraCIAPI for information about the vehicle or to issue commands.
 */
class Vehicle
{
    ns3::Ptr<ns3::Node> vehicle_node;
    ns3::NetDeviceContainer vehicle_devices;
    std::shared_ptr<VehicleAttributes> vehicle_attributes;
    std::string vehicle_id;
    int target_lane = -1;
    int previous_lane = -1;
    void VerifyLaneChange(int Lane_Index);
public:
    Vehicle(ns3::Ptr<ns3::Node> Vehicle_Node, ns3::NetDeviceContainer Vehicle_Devices,
            std::shared_ptr<VehicleAttributes> Vehicle_Attributes, std::string ID);
    ~Vehicle() = default;
    void Step(std::shared_ptr<TraCIClient> Client);
    ns3::Ptr<ns3::Node> GetNode();
    ns3::NetDeviceContainer& GetDevices();
    std::shared_ptr<VehicleAttributes> GetAttributes();
    std::string GetID();
    std::string GetIPAddress();
    void RecommendLaneChange(bool Recommendation, int Lane_Index, std::shared_ptr<TraCIClient> Client);
    void SetTarget(int Target_Lane);
    bool HasTarget();
    std::string ToString();
};

#endif
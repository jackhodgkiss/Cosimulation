#ifndef COSIMULATION_VEHICLEAPPLICATION_H
#define COSIMULATION_VEHICLEAPPLICATION_H

#include <memory>
#include <ns3/ptr.h>
#include "Vehicle.h"
#include <ns3/socket.h>
#include <ns3/application.h>
#include "VehicleAttributes.h"

/**
 * This enum represents the types of packets that maybe sent between vehicles within this VehicleApplication installed
 * upon them.
 * Get: Vehicles that receive this must interpret this as a request for information if they can be of assistance.
 * Response: Vehicles that receive this must interpret this as a response to packets of type 'Get'
 * Command: Vehicles that receive this must interpret this as a request to modify their speed to accommodate the
 * requesting vehicle.
 */
enum Context {Get, Response, Command};

/**
 * This class will act as the base of the two applications that shall be installed upon the vehicle node within this
 * simulation. This base will ensure that components are configured appropriately however implementation will be carried
 * out by this classes that derive this class.
 */
class VehicleApplication : public ns3::Application
{
private:
    ns3::Ptr<ns3::Socket> socket;
    std::map<ns3::Address, VehicleAttributes> responses;
    std::shared_ptr<Vehicle> vehicle;
    std::shared_ptr<TraCIClient> client;
    ns3::Time transmission_delay_ns;
protected:
    virtual void StartApplication();
    virtual void StopApplication() { };
    virtual void RunAlgorithm(int Lane_Index);
    virtual void Send(Context Action, std::string Content, ns3::Address Recipient);
    virtual Context Read(ns3::Ptr<ns3::Packet> Packet, std::string& Content);
    virtual void Receive(ns3::Ptr<ns3::Socket> Socket);
    bool GetPartner(std::pair<ns3::Address, VehicleAttributes>& Partner);
    bool IsPresent();
    ns3::Ptr<ns3::Socket> GetSocket();
    std::map<ns3::Address, VehicleAttributes>& GetResponses();
    std::shared_ptr<Vehicle> GetVehicle();
    std::string GetVehicleID();
    std::shared_ptr<VehicleAttributes> GetVehicleAttributes();
    std::shared_ptr<TraCIClient> GetClient();
    ns3::Time GetTransmissionDelay();
public:
    VehicleApplication() = default;
    ~VehicleApplication() = default;
    virtual void ChangeLane(int Lane_Index);
    void Install(std::shared_ptr<Vehicle> Vehicle, std::shared_ptr<TraCIClient> Client);
};

#endif
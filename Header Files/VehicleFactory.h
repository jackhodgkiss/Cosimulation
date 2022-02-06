#ifndef COSIMULATION_VEHICLEFACTORY_H
#define COSIMULATION_VEHICLEFACTORY_H

#include <memory>
#include <string>
#include "Vehicle.h"
#include <ns3/wave-module.h>
#include <ns3/wifi-module.h>
#include <ns3/mobility-module.h>
#include <ns3/ipv4-address-helper.h>

/**
 * This class is responsible for constructing vehicles to specification for use within the simulations employed by this
 * application. This class almost exclusively concerns itself with configuring the vehicle for NS-3 with it establishing
 * a network device capable of simulating the IEEE 802.11p standard.
 */
class VehicleFactory
{
    std::string address_base;
    std::string subnet_mask;
    std::string data_mode = "OfdmRate6MbpsBW10MHz";
    std::string control_mode = "OfdmRate6MbpsBW10MHz";
    ns3::Ipv4AddressHelper address_helper;
    ns3::YansWifiChannelHelper channel_helper;
    ns3::YansWavePhyHelper physical_helper;
    ns3::NqosWaveMacHelper mac_helper;
    ns3::Wifi80211pHelper wifi_helper;
    ns3::InternetStackHelper stack_helper;
    ns3::MobilityHelper mobility_helper;
    ns3::Vector initial_position = ns3::Vector(0, 0, 10000);
public:
    explicit VehicleFactory(std::string Address_Base = "10.0.0.0", std::string Subnet_Mask = "255.0.0.0");
    ~VehicleFactory() = default;
    std::shared_ptr<Vehicle> CreateVehicle(std::string ID);
};

#endif
#include "../Header Files/VehicleFactory.h"

using namespace ns3;

/**
 * Construct a factory capable of producing vehicles configured to meet the needs of the application. The factory can be
 * supplied with an address base and subnet mask create different networks.
 * @param Address_Base Starting address used by the address helper.
 * @param Subnet_Mask Subnet mask used to create subdivisions within the network.
 */
VehicleFactory::VehicleFactory(std::string Address_Base, std::string Subnet_Mask)
{
    this->address_base = Address_Base;
    this->subnet_mask = Subnet_Mask;
    this->address_helper.SetBase(this->address_base.c_str(), this->subnet_mask.c_str());
    this->channel_helper = YansWifiChannelHelper::Default();
    this->physical_helper = YansWavePhyHelper::Default();
    this->physical_helper.SetChannel(this->channel_helper.Create());
    this->mac_helper = NqosWaveMacHelper::Default();
    this->wifi_helper = Wifi80211pHelper::Default();
    this->wifi_helper.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                              "DataMode", StringValue(this->data_mode),
                                              "ControlMode", StringValue(this->control_mode));
    this->mobility_helper.SetMobilityModel("ns3::WaypointMobilityModel");
}

/**
 * Construct a new vehicle with the next available IP address and the provided unique identifier.
 * @param ID Unique identifier used to interact with SUMO/TraCI.
 * @return Newly constructed vehicle inside a unique ptr.
 */
std::shared_ptr<Vehicle> VehicleFactory::CreateVehicle(std::string ID)
{
    Ptr<Node> node = CreateObject<Node>();
    this->mobility_helper.Install(node);
    node->GetObject<WaypointMobilityModel>()->AddWaypoint(Waypoint(Seconds(0), this->initial_position));
    NetDeviceContainer devices = this->wifi_helper.Install(this->physical_helper, this->mac_helper, node);
    std::shared_ptr<VehicleAttributes> attributes = std::make_shared<VehicleAttributes>();
    this->stack_helper.Install(node);
    this->address_helper.Assign(devices);
    return std::make_shared<Vehicle>(node, devices, attributes, ID);
}

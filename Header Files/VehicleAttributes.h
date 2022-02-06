#ifndef COSIMULATION_VEHICLEATTRIBUTES_H
#define COSIMULATION_VEHICLEATTRIBUTES_H

#include <string>
#include <libsumo/TraCIDefs.h>
#include <utils/traci/TraCIAPI.h>

/**
 * This struct will be responsible for representing all of the attributes associated with vehicles from the SUMO side of
 * this simulation. These values can be accessed to enable NS-3 the ability to position the nodes and network devices
 * appropriately and empower the algorithms to make valid and accurate decisions. This struct also provides the ability
 * to update the values with a TraCIValues object obtained via a TraCI subscription.
 */
struct VehicleAttributes
{
    double Speed;
    libsumo::TraCIPosition Position;
    int Lane_Index;
    double Length;
    double Max_Speed;
    double Acceleration;
    double Deceleration;
    double Max_Legal_Speed;
    std::vector<int> Attribute_Names = {VAR_SPEED, VAR_POSITION,
                                        VAR_LANE_INDEX, VAR_LENGTH,
                                        VAR_MAXSPEED, VAR_ACCEL,
                                        VAR_DECEL, VAR_ALLOWED_SPEED};
    void Update(TraCIAPI::TraCIValues Results);
    VehicleAttributes(double Speed = 0, libsumo::TraCIPosition Position = libsumo::TraCIPosition(),
                      int Lane_Index = 0, double Length = 0,
                      double Max_Speed = 0, double Acceleration = 0,
                      double Deceleration = 0, double Max_Legal_Speed = 0);
    ~VehicleAttributes() = default;
    static std::string Serialise(VehicleAttributes Vehicle_Attributes);
    static VehicleAttributes Deserialise(std::string Data);
    std::string ToString();
};

#endif
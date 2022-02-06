#include "../Header Files/TraCIClient.h"

/**
 * Specify a new lane change mode for a given vehicle within the SUMO simulation.
 * @param Vehicle_ID Unique identifier of the vehicle to change lane.
 * @param Mode The new mode that will be applied to the vehicle's lane change model.
 * http://sumo.dlr.de/wiki/TraCI/Change_Vehicle_State#lane_change_mode_.280xb6.29
 */
void TraCIClient::SetLaneChangeMode(std::string Vehicle_ID, int Mode)
{
    tcpip::Storage storage;
    storage.writeUnsignedByte(TYPE_INTEGER);
    storage.writeInt(Mode);
    this->send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_LANECHANGE_MODE, Vehicle_ID, storage);
    tcpip::Storage message;
    this->check_resultState(message, CMD_SET_VEHICLE_VARIABLE);
}

/**
 * Issue the command to instruct the vehicle to change into the specified lane.
 * @param Vehicle_ID Unique identifier of the vehicle to change lane.
 * @param Lane_Index The index of the lane to change to.
 * @param Duration The amount of time the lane shall be chosen for? SUMO/TraCIAPI documentation not clear!
 * http://sumo.dlr.de/wiki/TraCI/Change_Vehicle_State#change_lane_.280x13.29
 */
void TraCIClient::ChangeLane(std::string Vehicle_ID, int Lane_Index, SUMOTime Duration)
{
    tcpip::Storage storage;
    storage.writeUnsignedByte(TYPE_COMPOUND);
    storage.writeInt(2);
    storage.writeUnsignedByte(TYPE_BYTE);
    storage.writeByte(Lane_Index);
    storage.writeUnsignedByte(TYPE_INTEGER);
    storage.writeInt((int)Duration);
    this->send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, CMD_CHANGELANE, Vehicle_ID, storage);
    tcpip::Storage message;
    this->check_resultState(message, CMD_SET_VEHICLE_VARIABLE);
}

/**
 * Change the maximum speed limit of a given lane.
 * @param Lane_ID Unique identifier of the lane whose speed is to change.
 * @param New_Speed The new max speed applied to the lane.
 */
void TraCIClient::ChangeLaneSpeedLimit(std::string Lane_ID, double New_Speed)
{
    tcpip::Storage storage;
    storage.writeUnsignedByte(TYPE_DOUBLE);
    storage.writeDouble(New_Speed);
    this->send_commandSetValue(CMD_SET_LANE_VARIABLE, VAR_MAXSPEED, Lane_ID, storage);
    tcpip::Storage message;
    this->check_resultState(message, CMD_SET_LANE_VARIABLE);
}

#ifndef COSIMULATION_CONFIGURATION_H
#define COSIMULATION_CONFIGURATION_H

#include <bitset>
#include <string>
#include <vector>
#include <ns3/core-module.h>

/**
 * This struct is responsible for representing the configuration of the experiment being carried out. This will function
 * by having the user provide a series of configuration values that will augment the simulation producing different
 * results that can be used to gain new insight. Once the configuration has been constructed values maybe called upon to
 * alter objects outside this configuration struct.
 */
struct Configuration
{
    double Step_Length = 1;
    std::string SUMO_URL = "";
    std::string Remote_Address = "";
    int Remote_Port = 1337;
    std::string Animation_URL = "";
    std::bitset<5> Selection_Lanes;
    double Selection_Probability = 0.01;
    int Selection_Interval = 2;
    int Seed = 38203494;
    std::vector<double> Lane_Speed_Limits = {26.82, 26.82, 26.82, 26.82, 26.82};
    std::string Lane_Change_Output;
    std::string Trip_Info_Output;
    bool Use_Enhanced = false;
    Configuration(int argc, char** argv);
    ~Configuration() = default;
private:
    ns3::CommandLine command_line;
    bool SetStepLength(std::string Value);
    bool SetSUMOURL(std::string Value);
    bool SetRemoteAddress(std::string Value);
    bool SetRemotePort(std::string Value);
    bool SetAnimate(std::string Value);
    bool SetSelectionLanes(std::string Value);
    bool SetSelectionProbability(std::string Value);
    bool SetSelectionInterval(std::string Value);
    bool SetSeed(std::string Value);
    bool SetSpeedLimits(std::string Value);
    bool SetLaneChangeOutput(std::string);
    bool SetTripInfoOutput(std::string);
    bool SetUseEnhanced(std::string);
};

#endif
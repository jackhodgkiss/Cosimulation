#include "../Header Files/VehicleAttributes.h"

/**
 * Construct a new struct that will hold all of the needed attributes to make this simulation function.
 * @param Speed Current speed of the vehicle.
 * @param Position Current position of the vehicle.
 * @param Lane_Index The lane that vehicle is currently occupying.
 * @param Length The length of the vehicle.
 * @param Max_Speed The max physical speed of the vehicle.
 * @param Acceleration The acceleration rate of the vehicle.
 * @param Deceleration The deceleration rate of the vehicle.
 * @param Max_Legal_Speed Max legal speed that vehicle can travel at currently.
 */
VehicleAttributes::VehicleAttributes(double Speed, libsumo::TraCIPosition Position, int Lane_Index, double Length,
                                     double Max_Speed, double Acceleration, double Deceleration, double Max_Legal_Speed)
{
    this->Speed = Speed;
    this->Position = Position;
    this->Lane_Index = Lane_Index;
    this->Length = Length;
    this->Max_Speed = Max_Speed;
    this->Acceleration = Acceleration;
    this->Deceleration = Deceleration;
    this->Max_Legal_Speed = Max_Legal_Speed;
}

/**
 * Update all attributes contained within this struct with results obtained via a subscription.
 * @param Results Container of results requested in the subscription.
 */
void VehicleAttributes::Update(TraCIAPI::TraCIValues Results)
{
    this->Speed = Results.at(this->Attribute_Names.at(0)).scalar;
    this->Position = Results.at(this->Attribute_Names.at(1)).position;
    this->Lane_Index = (int)Results.at(this->Attribute_Names.at(2)).scalar;
    this->Length = Results.at(this->Attribute_Names.at(3)).scalar;
    this->Max_Speed = Results.at(this->Attribute_Names.at(4)).scalar;
    this->Acceleration = Results.at(this->Attribute_Names.at(5)).scalar;
    this->Deceleration = Results.at(this->Attribute_Names.at(6)).scalar;
    this->Max_Legal_Speed = Results.at(this->Attribute_Names.at(7)).scalar;
}

/**
 * Serialise the attributes stored within the supplied instance of VehicleAttributes
 * @param Vehicle_Attributes Struct of attributes to be serialised.
 * @return String of serialised data.
 */
std::string VehicleAttributes::Serialise(VehicleAttributes Vehicle_Attributes)
{
    char buffer[256];
    std::sprintf(buffer, "Speed: %.2f\nX: %.2f\nY: %.2f\nLane Index: %d\n"
                         "Length: %.2f\nMax Speed: %.2f\nAcceleration: %.2f\n"
                         "Deceleration: %.2f\nMax Legal Speed: %.2f", Vehicle_Attributes.Speed,
                 Vehicle_Attributes.Position.x, Vehicle_Attributes.Position.y,
                 Vehicle_Attributes.Lane_Index, Vehicle_Attributes.Length,
                 Vehicle_Attributes.Max_Speed, Vehicle_Attributes.Acceleration,
                 Vehicle_Attributes.Deceleration, Vehicle_Attributes.Max_Legal_Speed);
    return std::string(buffer);
}

/**
 * Deserialise a string of attribute data into a VehicleAttributes instance.
 * @param Data Serialised string of attributes.
 * @return Instance of serialised data deserialised into a vehicle attributes instance.
 */
VehicleAttributes VehicleAttributes::Deserialise(std::string Data)
{
    std::stringstream stream(Data);
    double speed = 0;
    libsumo::TraCIPosition position = libsumo::TraCIPosition();
    int lane_index = 0;
    double length = 0;
    double max_speed = 0;
    double acceleration = 0;
    double deceleration = 0;
    double max_legal_speed = 0;
    for(std::string line; std::getline(stream, line);)
    {
        if(line.compare(0, 5, "Speed") == 0)
        {
            speed = std::stod(line.substr(7, line.back()));
        }
        else if(line.compare(0, 1, "X") == 0)
        {
            position.x = std::stod(line.substr(3, line.back()));
        }
        else if(line.compare(0, 1, "Y") == 0)
        {
            position.y = std::stod(line.substr(3, line.back()));
        }
        else if(line.compare(0, 10, "Lane Index") == 0)
        {
            lane_index = std::stoi(line.substr(12, line.back()));
        }
        else if(line.compare(0, 6, "Length") == 0)
        {
            length = std::stod(line.substr(8, line.back()));
        }
        else if(line.compare(0, 9, "Max Speed") == 0)
        {
            max_speed = std::stod(line.substr(11, line.back()));
        }
        else if(line.compare(0, 12, "Acceleration") == 0)
        {
            acceleration = std::stod(line.substr(14, line.back()));
        }
        else if(line.compare(0, 12, "Deceleration") == 0)
        {
            deceleration = std::stod(line.substr(14, line.back()));
        }
        else if(line.compare(0, 15, "Max Legal Speed") == 0)
        {
            max_legal_speed = std::stod(line.substr(17, line.back()));
        }
    }
    return VehicleAttributes(speed, position, lane_index, length, max_speed, acceleration, deceleration, max_legal_speed);
}

/**
 * Get the current state of the attributes.
 * @return Current state of the attributes.
 */
std::string VehicleAttributes::ToString()
{
    char buffer[256];
    std::sprintf(buffer, "Speed: %.2f, Position: (X: %.2f, Y: %.2f), Lane Index: %d, "
                         "Length: %.2f, Max Speed: %.2f, Acceleration: %.2f, "
                         "Deceleration: %.2f, Max Legal Speed: %.2f", this->Speed,
                 this->Position.x, this->Position.y,
                 this->Lane_Index, this->Length,
                 this->Max_Speed, this->Acceleration,
                 this->Deceleration, this->Max_Legal_Speed);
    return std::string(buffer);
}
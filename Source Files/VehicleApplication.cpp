#include "../Header Files/VehicleApplication.h"
#include <algorithm>
#include <ns3/ipv4.h>
#include <ns3/core-module.h>

using namespace ns3;

/**
 * Start the application. This will where any initialisation takes place.
 */
void VehicleApplication::StartApplication()
{
    TypeId type_id = TypeId::LookupByName("ns3::UdpSocketFactory");
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 80);
    InetSocketAddress remote = InetSocketAddress("10.255.255.255", 80);
    this->socket = Socket::CreateSocket(this->GetNode(), type_id);
    this->socket->SetRecvCallback(MakeCallback(&VehicleApplication::Receive, this));
    this->socket->SetAllowBroadcast(true);
    this->socket->Bind(local);
    this->socket->Connect(remote);
    Ptr<UniformRandomVariable> random_generator = CreateObject<UniformRandomVariable>();
    this->transmission_delay_ns = NanoSeconds(random_generator->GetInteger(0, 50));
}

/**
 * Used to express the desire to change lane. This is used to run the algorithm contained within the application.
 * @param Lane_Index Index of the lane the vehicle desires to change to.
 */
void VehicleApplication::ChangeLane(int Lane_Index) { }

/**
 * The algorithm will be contained here in the child implementations of this class. This algorithm will coordinate a
 * lane change maneuver to a desired lane exploiting wireless facilities on the vehicles within this simulation.
 * @param Lane_Index Index of the lane the vehicle desires to change to.
 */
void VehicleApplication::RunAlgorithm(int Lane_Index) { }

/**
 * Construct and send a packet to a either a specified recipient or all vehicles within the communication range.
 * @param Action The action recipients must carryout upon them receiving the packet.
 * @param Content Content of the packet containing instructions or information.
 * @param Recipient If the packet is to be sent to an individual vehicle then the address can be specified.
 */
void VehicleApplication::Send(Context Action, std::string Content, Address Recipient)
{
    char buffer[Content.size() + 16];
    std::string action_name = Action == Get ? "Get" : Action == Response ? "Response" : "Command";
    std::sprintf(buffer, "%s/%s", action_name.c_str(), Content.c_str());
    std::string message = std::string(buffer);
    Ptr<Packet> packet = Create<Packet>((uint8_t*)message.c_str(), message.size() + 1);
    if(Action == Get)
    {
        this->socket->Send(packet);
    }
    else
    {
        this->socket->SendTo(packet, 0, Recipient);
    }
}

/**
 * Read the packet to determine the Context and Content of it.
 * @param Packet Network packet received by the network device.
 * @param Content Buffer which the content can be appended to.
 * @return Context of the packet.
 */
Context VehicleApplication::Read(Ptr<Packet> Packet, std::string& Content)
{
    Context result = Get;
    uint8_t buffer[Packet->GetSize()];
    Packet->CopyData(buffer, Packet->GetSize());
    std::string payload = std::string((char*)buffer);
    if(payload.compare(0, 3, "Get") == 0)
    {
        result = Get;
        Content.append(payload.substr(4, payload.back()));
    }
    else if(payload.compare(0, 8, "Response") == 0)
    {
        result = Response;
        Content.append(payload.substr(9, payload.size() - 9));
    }
    else if(payload.compare(0, 7, "Command") == 0)
    {
        result = Command;
        Content.append(payload.substr(8, payload.size() - 8));
    }
    return result;
}

/**
 * This is the function that is called upon every time the network device receives a packet.
 * @param Socket The socket which was targeted by the sender.
 */
void VehicleApplication::Receive(Ptr<Socket> Socket) { }

/**
 * Get the partner in the lane desired by this vehicle. Partner can be defined as the vehicle whose position and speed
 * may determine the outcome of the lane change request.
 * @param Partner Container of address and attributes of the partner.
 * @return True if a partner has been identified if not return false.
 */
bool VehicleApplication::GetPartner(std::pair<Address, VehicleAttributes>& Partner)
{
    bool result = false;
    if(!this->responses.empty())
    {
        double position_x = this->GetVehicleAttributes()->Position.x;
        std::map<Address, VehicleAttributes> nearby_or_behind;
        for(const auto& pair : this->responses)
        {
            if((pair.second.Position.x + (2 * pair.second.Length)) < position_x)
            {
                nearby_or_behind.insert(std::pair<Address, VehicleAttributes>(pair.first, pair.second));
            }
        }
        if(!nearby_or_behind.empty())
        {
            Partner.first = nearby_or_behind.begin()->first;
            Partner.second = nearby_or_behind.begin()->second;
            for(const auto& pair : nearby_or_behind)
            {
                if(abs((int)(pair.second.Position.x - position_x)) <=
                   abs((int)(Partner.second.Position.x - position_x)))
                {
                    Partner.first = pair.first;
                    Partner.second = pair.second;
                }
            }
            result = true;
        }
    }
    return result;
}

/**
 * Determine if the vehicle is still active within SUMO. Used to prevent requests to change lane or slow down to
 * vehicles that have just left the SUMO simulation.
 * @return True if the vehicle if the vehicle is still in SUMO else false.
 */
bool VehicleApplication::IsPresent()
{
    std::vector<std::string> id_list = this->GetClient()->vehicle.getIDList();
    return std::find(id_list.begin(), id_list.end(), this->GetVehicleID()) != id_list.end();
}

/**
 * Get the socket used by this application.
 * @return Socket used by this application.
 */
Ptr<Socket> VehicleApplication::GetSocket()
{
    return this->socket;
}

/**
 * Get the responses obtained after sending a request to get information from vehicles that maybe of assistance.
 * @return Map of responses collected.
 */
std::map<Address, VehicleAttributes>& VehicleApplication::GetResponses()
{
    return this->responses;
}

/**
 * Get the vehicle associated with this application.
 * @return Vehicle associated with this application.
 */
std::shared_ptr<Vehicle> VehicleApplication::GetVehicle()
{
    return this->vehicle;
}

/**
 * Get the vehicle's unique identifier associated with this application.
 * @return Vehicle's unique identifier associated with this application.
 */
std::string VehicleApplication::GetVehicleID()
{
    return this->vehicle->GetID();
}

/**
 * Get the attributes of the vehicle associated with this application.
 * @return Attributes of the vehicle associated with this application.
 */
std::shared_ptr<VehicleAttributes> VehicleApplication::GetVehicleAttributes()
{
    return this->vehicle->GetAttributes();
}

/**
 * Get the TraCIClient to interact with the SUMO simulation.
 * @return Instance of TraCIClient connected to the SUMO simulation.
 */
std::shared_ptr<TraCIClient> VehicleApplication::GetClient()
{
    return this->client;
}

/**
 * Get the amount of time in nano seconds which should used to delay the transmission of packets.
 * @return Delay on the transmission of packets.
 */
Time VehicleApplication::GetTransmissionDelay()
{
    return this->transmission_delay_ns;
}

/**
 * Install the application upon the vehicle.
 * @param Vehicle Vehicle this application should be install on.
 * @param Client TraCIClient connected to SUMO simulation.
 */
void VehicleApplication::Install(std::shared_ptr<Vehicle> Vehicle, std::shared_ptr<TraCIClient> Client)
{
    this->vehicle = Vehicle;
    this->client = Client;
    this->SetStartTime(Seconds(0));
    this->vehicle->GetNode()->AddApplication(this);
}


cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(Cosimulation)

set(CMAKE_CXX_STANDARD 11)

set(NS3_BUILD /home/jack/NS3/ns-3.28/build)
set(SUMO_BUILD /home/jack/SUMO/src)
include_directories(${NS3_BUILD}/ ${SUMO_BUILD}/)
link_directories(${NS3_BUILD} ${SUMO_BUILD})

set(HEADER_FILES "Header Files/TraCIClient.h" "Header Files/Vehicle.h"
        "Header Files/VehicleFactory.h" "Header Files/VehicleAttributes.h"
        "Header Files/Experiment.h" "Header Files/Configuration.h"
        "Header Files/Governor.h" "Header Files/VehicleApplication.h"
        "Header Files/ILACHApplication.h" "Header Files/ILACHPlusApplication.h")
set(SOURCE_FILES "Source Files/Main.cpp" "Source Files/TraCIClient.cpp"
        "Source Files/Vehicle.cpp" "Source Files/VehicleFactory.cpp"
        "Source Files/VehicleAttributes.cpp" "Source Files/Experiment.cpp"
        "Source Files/Configuration.cpp" "Source Files/Governor.cpp"
        "Source Files/VehicleApplication.cpp" "Source Files/ILACHApplication.cpp" "Source Files/ILACHPlusApplication.cpp")
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
        ns3.28-core-debug
        ns3.28-wave-debug
        ns3.28-wifi-debug
        ns3.28-netanim-debug
        ns3.28-network-debug
        ns3.28-internet-debug
        ns3.28-mobility-debug
        ns3.28-applications-debug
        ${SUMO_BUILD}/foreign/tcpip/socket.o
        ${SUMO_BUILD}/foreign/tcpip/storage.o
        ${SUMO_BUILD}/utils/traci/libtraciclient.a)
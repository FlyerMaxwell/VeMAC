cmake_minimum_required(VERSION 3.22)
project(VeMAC_INFOCOM)

set(CMAKE_CXX_STANDARD 14)

add_executable(VeMAC_INFOCOM main.cpp common.cpp common.h UpLocation.cpp UpLocation.h vehicle.cpp vehicle.h parameters.cpp parameters.h Protocol_VeMAC.cpp Protocol_VeMAC.h communication.cpp communication.h packet.cpp packet.h collision.cpp collision.h)

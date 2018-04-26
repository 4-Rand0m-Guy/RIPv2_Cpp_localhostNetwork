cmake_minimum_required(VERSION 3.10)
project(RIP_)

set(CMAKE_CXX_STANDARD 11)

add_executable(RIP_ main.cpp config/ConfigImporter.cpp config/ConfigImporter.h config/OutputInterface.cpp config/OutputInterface.h utils/Utils.cpp utils/Utils.h config/ConsoleLogger.cpp config/ConsoleLogger.h rip/Rip.cpp rip/Rip.h config/Configuration.cpp config/Configuration.h rip/RIPHeader.cpp rip/RIPHeader.h config/RIPRouteEntry.cpp config/RIPRouteEntry.h)
cmake_minimum_required(VERSION 3.10)
project(RIP_)

set(CMAKE_CXX_STANDARD 17)

add_executable(RIP_ main.cpp config/ConfigImporter.cpp config/ConfigImporter.h config/OutputInterface.cpp config/OutputInterface.h utils/Utils.cpp utils/Utils.h config/ConsoleLogger.cpp config/ConsoleLogger.h rip/Rip.cpp rip/Rip.h config/Configuration.cpp config/Configuration.h rip/rip_client_server.cpp rip/rip_client_server.h)
cmake_minimum_required(VERSION 3.10)
project(RIP_)

set(CMAKE_CXX_STANDARD 11)

add_executable(RIP_ main.cpp ConfigImporter.cpp ConfigImporter.h OutputInterface.cpp OutputInterface.h Utils.cpp Utils.h)
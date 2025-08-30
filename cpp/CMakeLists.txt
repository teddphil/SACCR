
cmake_minimum_required(VERSION 3.10)
project(saccr_demo LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(saccr_demo src/main.cpp src/market.cpp src/trades.cpp src/montecarlo.cpp src/saccr.cpp)

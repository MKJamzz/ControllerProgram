cmake_minimum_required(VERSION 3.15)
project(ControllerProgram)

set(CMAKE_CXX_STANDARD 17)

# ---- Client (Laptop) ----
# SDL2 only needed on laptop

# find_package(SDL2 REQUIRED)
# add_executable(LaptopClient LaptopClient.cpp)
# target_link_libraries(LaptopClient PRIVATE SDL2::SDL2 ws2_32)

# ---- Server (Raspberry Pi) ----
# # pigpio only on the Pi
add_executable(CarServer CarServer.cpp)
target_link_libraries(CarServer PRIVATE pigpio rt)

# ---- Original RCCarController if needed (Pi only) ----
# add_executable(RCCarController main.cpp)
# target_link_libraries(RCCarController PRIVATE cpr::cpr SDL2::SDL2 OpenSSL::SSL OpenSSL::Crypto gpiod)

cmake_minimum_required(VERSION 3.15)
project(RCCarController)

set(CMAKE_CXX_STANDARD 17)

# Path to vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Find CPR and SDL2
find_package(cpr CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

# Build the executable
add_executable(RCCarController main.cpp)

# Link the libraries
target_link_libraries(RCCarController PRIVATE cpr::cpr SDL2::SDL2)

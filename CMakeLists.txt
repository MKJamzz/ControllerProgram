cmake_minimum_required(VERSION 3.15)
project(RCCarController)

set(CMAKE_CXX_STANDARD 17)

# vcpkg toolchain and triplet
set(CMAKE_TOOLCHAIN_FILE "C:/Users/pugpo/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_TARGET_TRIPLET "x64-mingw-static")
set(cpr_DIR "C:/Users/pugpo/dev/vcpkg/installed/x64-mingw-static/share/cpr")
set(CURL_DIR "C:/Users/pugpo/dev/vcpkg/installed/x64-mingw-static/share/curl")
set(SDL2_DIR "C:/Users/pugpo/dev/vcpkg/installed/x64-mingw-static/share/sdl2")




# find packages
find_package(cpr CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_executable(RCCarController main.cpp)
target_link_libraries(RCCarController PRIVATE cpr::cpr SDL2::SDL2)



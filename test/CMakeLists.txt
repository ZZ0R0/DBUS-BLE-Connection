cmake_minimum_required(VERSION 3.10)

# Set the project name
project(BLETest)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)

# Find and link D-Bus using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)

# Add the main.cpp
add_executable(test_ble main.cpp)

# Specify the path to the shared library
target_link_libraries(test_ble PRIVATE ${CMAKE_SOURCE_DIR}/../framework/build/libBLEFramework.so)

# Include the headers from the BLEFramework
target_include_directories(test_ble PRIVATE ${CMAKE_SOURCE_DIR}/../include/BLEFramework)

# Include D-Bus directories
target_include_directories(test_ble PRIVATE ${DBUS_INCLUDE_DIRS})

# Link with D-Bus libraries
target_link_libraries(test_ble PRIVATE ${DBUS_LIBRARIES})

# Add pthread for multithreading support
target_link_libraries(test_ble PRIVATE pthread)

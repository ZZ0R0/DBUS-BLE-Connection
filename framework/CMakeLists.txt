# framework/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(BLEFramework)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)

# Include directories
include_directories(
    ${DBUS_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/BLEFramework
)

# Link directories
link_directories(${DBUS_LIBRARY_DIRS})

# Create the shared library
add_library(BLEFramework SHARED
    ../src/BLEManager.cpp
    ../src/DbusConnection.cpp
    ../src/CharacteristicManager.cpp
    ../src/PipeManager.cpp
    ../src/DeviceManager.cpp
    ../src/Utils.cpp
)

# Specify public headers
target_include_directories(BLEFramework PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

# Link libraries
target_link_libraries(BLEFramework
    ${DBUS_LIBRARIES}
)

# Optional: Installation rules
install(TARGETS BLEFramework
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ../include/ DESTINATION include)

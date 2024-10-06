cmake_minimum_required(VERSION 3.10)
project(BLEProject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)

include_directories(
    ${DBUS_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

link_directories(${DBUS_LIBRARY_DIRS})

add_executable(ble
    src/main.cpp
    src/BLEManager.cpp
    src/DbusConnection.cpp
    src/CharacteristicManager.cpp
    src/ESP32PipeManager.cpp
    src/DeviceManager.cpp
    src/Utils.cpp
)

target_link_libraries(ble
    ${DBUS_LIBRARIES}
)

# embedded/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(BLEProjectTest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)

# Include directories: DBUS and framework's headers
include_directories(
    ${DBUS_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/BLEFramework
)

# Link directories
link_directories(${DBUS_LIBRARY_DIRS})

# Add the executable, including embedded's main.cpp and framework's source files
add_executable(ble
    main.cpp
    ../src/BLEManager.cpp
    ../src/DbusConnection.cpp
    ../src/CharacteristicManager.cpp
    ../src/PipeManager.cpp
    ../src/DeviceManager.cpp
    ../src/Utils.cpp
)

# Link against DBUS libraries
target_link_libraries(ble
    ${DBUS_LIBRARIES}
)

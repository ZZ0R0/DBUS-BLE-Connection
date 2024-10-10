// src/DeviceManager.cpp

#include "DeviceManager.h"
#include "BLEManager.h"
#include <iostream>

DeviceManager::DeviceManager(BLEManager &bleMgr)
    : bleManager(bleMgr)
{
    std::cout << "[DeviceManager] Constructor called." << std::endl;
}

DeviceManager::~DeviceManager()
{
    std::cout << "[DeviceManager] Destructor called." << std::endl;
}

// List all connected devices using BLEManager
std::vector<BluetoothDevice> DeviceManager::listConnectedDevices()
{
    std::cout << "[DeviceManager] Retrieving connected devices..." << std::endl;

    // Call the low-level function in BLEManager
    std::vector<BluetoothDevice> devices = bleManager.listConnectedDevices();

    // Print out the devices
    for (const auto &device : devices)
    {
        std::cout << "Device Found: " << device.name << " (" << device.path << ")" << std::endl;
    }

    return devices;
}

// Connect to the selected device without handshake
bool DeviceManager::selectDevice()
{
    std::cout << "[DeviceManager] Selecting device..." << std::endl;

    std::vector<BluetoothDevice> devices = bleManager.listConnectedDevices();
    if (devices.empty())
    {
        std::cerr << "[DeviceManager] No connected devices available." << std::endl;
        return false;
    }

    // Select the first available device (or allow the user to choose)
    BluetoothDevice device = devices.front();
    bleManager.setSelectedDevicePath(device.path);
    std::cout << "[DeviceManager] Selected Device: " << device.name << " (" << device.path << ")" << std::endl;

    // Establish connection to the device without performing handshake
    if (!bleManager.connectToDevice(device.macAddress))
    {
        std::cerr << "[DeviceManager] Failed to connect to device: " << device.name << std::endl;
        return false;
    }

    return true;
}

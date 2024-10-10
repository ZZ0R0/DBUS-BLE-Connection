// src/DeviceManager.h

#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "BLETypes.h"
#include "BLEManager.h"
#include <vector>

class DeviceManager
{
public:
    DeviceManager(BLEManager &bleMgr);
    ~DeviceManager();

    // Select a device (e.g., based on user input or automatically)
    bool selectDevice();

    // List all connected devices (calls BLEManager's function)
    std::vector<BluetoothDevice> listConnectedDevices();

private:
    BLEManager &bleManager;
};

#endif // DEVICEMANAGER_H

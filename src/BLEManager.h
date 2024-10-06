// src/BLEManager.h

#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <string>
#include <vector>
#include "BLETypes.h"
#include "DbusConnection.h"
#include "CharacteristicManager.h"
#include "ESP32PipeManager.h"

class BLEManager

{

public:
    BLEManager();

    ~BLEManager();

    bool initialize();

    bool connectToDevice(const BluetoothDevice &device);

    std::vector<BluetoothDevice> listConnectedDevices();

    bool selectDevice();

    bool listAllCharacteristics();

    bool performHandshake();

    bool sendMessage(const std::string &message);

    bool receiveMessage(std::string &message);

    bool printObjectTree(const std::string &objectPath, int indent);

    std::string getSelectedDevicePath() const;

    void setSelectedDevicePath(const std::string &devicePath);

    ESP32Pipes getESP32Pipes() const;

    void listAvailableMethods(const std::string &objectPath); // Add this line

private:
    DbusConnection *dbusConn;

    CharacteristicManager *charManager;

    ESP32PipeManager *pipeManager;

    std::string selectedDevicePath;
};

#endif // BLEMANAGER_H
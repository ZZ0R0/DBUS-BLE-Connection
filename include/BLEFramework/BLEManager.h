// src/BLEManager.h

#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <string>
#include <vector>
#include <functional> // For std::function
#include "BLETypes.h"
#include "DbusConnection.h"
#include "CharacteristicManager.h"
#include "PipeManager.h" // Updated include

class BLEManager
{
public:
    BLEManager();

    ~BLEManager();

    // Initialize BLE Manager
    bool initialize();

    // Connect to a specific BLE device by MAC address
    bool connectToDevice(const std::string &macAddress);

    // List all connected devices
    std::vector<BluetoothDevice> listConnectedDevices();

    // Select a device (e.g., based on user input or automatically)
    bool selectDevice();

    // List all characteristics and populate uuidToPathMap
    bool listAllCharacteristics();

    // Send a message to the BLE device
    bool sendMessage(const std::string &message);

    // Receive a message from the BLE device
    bool receiveMessage(std::string &message);

    // Print the object tree for debugging
    bool printObjectTree(const std::string &objectPath, int indent);

    // Get the path of the selected device
    std::string getSelectedDevicePath() const;

    // Set the path of the selected device
    void setSelectedDevicePath(const std::string &devicePath);

    // List available methods for a given object path
    void listAvailableMethods(const std::string &objectPath); // Existing

    // Getter for CharacteristicManager
    CharacteristicManager *getCharacteristicManager() const;

    // Methods for dynamic pipe management
    void registerPipe(const BLEPipe &pipe);
    bool writeToPipe(const std::string &uuid, const std::string &data);
    bool readFromPipe(const std::string &uuid, std::string &data);

    // List all characteristics and pipes of the selected device
    bool initializeDevice();

    // Disconnect the BLE device
    void disconnectDevice();

private:
    DbusConnection *dbusConn;

    CharacteristicManager *charManager;

    PipeManager *pipeManager; // Updated to PipeManager

    std::string selectedDevicePath;

    // Additional private members as needed
};

#endif // BLEMANAGER_H

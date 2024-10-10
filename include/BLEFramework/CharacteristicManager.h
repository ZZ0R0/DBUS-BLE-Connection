// src/CharacteristicManager.h

#ifndef CHARACTERISTICMANAGER_H
#define CHARACTERISTICMANAGER_H

#include <dbus/dbus.h>
#include <string>
#include <map>
#include "BLETypes.h"

class DbusConnection; // Forward declaration

class CharacteristicManager
{
public:
    CharacteristicManager(DbusConnection &dbusConn, const std::string &devicePath);
    ~CharacteristicManager();

    // List all characteristics and populate uuidToPathMap
    bool listAllCharacteristics();

    // Write to a characteristic
    bool writeCharacteristic(const std::string &charPath, const std::string &value);

    // Read from a characteristic
    bool readCharacteristic(const std::string &charPath, std::string &value);

    // Getter for UUID to Path map
    std::map<std::string, std::string> getUuidToPathMap() const;

private:
    DbusConnection &dbusConnection;
    std::string devicePath;
    std::map<std::string, std::string> uuidToPathMap;
};

#endif // CHARACTERISTICMANAGER_H

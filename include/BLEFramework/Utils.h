#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "DbusConnection.h" // Include the DbusConnection header
#include "BLETypes.h"

class Utils
{
public:
    static BluetoothDevice parseBluetoothDevice(const std::string &objectPath, DbusConnection *dbusConn);
    static std::vector<std::string> extractChildPaths(const std::string &xmlData, const std::string &parentPath);
    static std::string toLower(const std::string &str);
};

#endif // UTILS_H

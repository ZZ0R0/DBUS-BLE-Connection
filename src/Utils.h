#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "DbusConnection.h" // Include the DbusConnection header

class Utils
{
public:
    static BluetoothDevice parseBluetoothDevice(const std::string &objectPath, DbusConnection *dbusConn);
    static std::vector<std::string> extractChildPaths(const std::string &xmlData, const std::string &parentPath);
};

// Helper function to convert string to lowercase for case-insensitive comparison
std::string toLower(const std::string &str);

// Helper function to extract child object paths from Introspection XML
std::vector<std::string> extractChildPaths(const std::string &xml, const std::string &parentPath);

#endif // UTILS_H

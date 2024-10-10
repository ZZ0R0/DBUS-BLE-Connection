// src/Utils.cpp
#include "Utils.h"
#include "BLETypes.h" // Ensure this is included
#include <algorithm>  // Required for std::transform
#include <cctype>     // Required for std::tolower

// Implement parseBluetoothDevice
BluetoothDevice Utils::parseBluetoothDevice(const std::string &objectPath, DbusConnection *dbusConn)
{
    BluetoothDevice device;
    device.path = objectPath;

    // Implement actual parsing, placeholder
    device.name = "DeviceName";              // Placeholder
    device.macAddress = "00:11:22:33:44:55"; // Placeholder
    device.connected = true;                 // Placeholder

    return device;
}

// Implement extractChildPaths
std::vector<std::string> Utils::extractChildPaths(const std::string &xmlData, const std::string &parentPath)
{
    std::vector<std::string> childPaths;
    std::string searchStr = "<node name=\"";
    size_t pos = 0;

    while ((pos = xmlData.find(searchStr, pos)) != std::string::npos)
    {
        pos += searchStr.length();
        size_t end = xmlData.find("\"", pos);
        if (end == std::string::npos)
            break;

        std::string childName = xmlData.substr(pos, end - pos);
        std::string childPath = parentPath + "/" + childName;
        childPaths.push_back(childPath);
        pos = end;
    }

    return childPaths;
}

// Implement toLower
std::string Utils::toLower(const std::string &str)
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
    return lowerStr;
}

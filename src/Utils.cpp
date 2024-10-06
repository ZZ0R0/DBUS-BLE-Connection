// src/Utils.cpp
#include "BLEManager.h"
#include "DbusConnection.h"
#include "Utils.h"
#include <algorithm>

// Helper function to convert string to lowercase for case-insensitive comparison
std::string toLower(const std::string &str)
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
    return lowerStr;
}

// Helper function to extract child object paths from Introspection XML
std::vector<std::string> extractChildPaths(const std::string &xml, const std::string &parentPath)
{
    std::vector<std::string> childPaths;
    std::string searchStr = "<node name=\"";
    size_t pos = 0;

    while ((pos = xml.find(searchStr, pos)) != std::string::npos)
    {
        pos += searchStr.length();
        size_t end = xml.find("\"", pos);
        if (end == std::string::npos)
            break;

        std::string childName = xml.substr(pos, end - pos);
        std::string childPath = parentPath + "/" + childName;
        childPaths.push_back(childPath);
        pos = end;
    }

    return childPaths;
}

// Parse the object path and return a BluetoothDevice structure
BluetoothDevice Utils::parseBluetoothDevice(const std::string &objectPath, DbusConnection *dbusConn)
{
    BluetoothDevice device;
    device.path = objectPath;

    // Here, we should query the D-Bus for the device properties like name, address, etc.
    // This is a placeholder code assuming that we can retrieve a name from the object path
    // In a real-world scenario, you would query the properties of the device via D-Bus

    if (objectPath.find("dev_") != std::string::npos)
    {
        device.name = "Device " + objectPath.substr(objectPath.find_last_of('/') + 1);
    }
    else
    {
        device.name = "Unknown Device";
    }

    return device;
}
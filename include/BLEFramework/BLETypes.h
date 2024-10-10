// include/BLETypes.h

#ifndef BLETYPES_H
#define BLETYPES_H

#include <string>
#include <map>
#include <vector>

// Struct to hold Bluetooth device information
struct BluetoothDevice
{
    std::string path;
    std::string name;
    std::string macAddress; // Added MAC address
    bool connected;
};

// Struct to hold BLE characteristic information
struct BLECharacteristic
{
    std::string path;
    std::string uuid;
};

// Enum to define the type of data pipe
enum class PipeType
{
    Handshake_RX,
    Handshake_TX,
    Message,
    Log,
    Config,
    // Add more types as needed
};

// Struct to represent a generic BLE Pipe
struct BLEPipe
{
    std::string uuid;
    std::string path;
    PipeType type;
};

#endif // BLETYPES_H

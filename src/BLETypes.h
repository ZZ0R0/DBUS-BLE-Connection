// src/BLETypes.h

#ifndef BLETYPES_H
#define BLETYPES_H

#include <string>

// Struct to hold Bluetooth device information
struct BluetoothDevice
{
    std::string path;
    std::string name;
    bool connected;
};

// Struct to hold BLE characteristic information
struct BLECharacteristic
{
    std::string path;
    std::string uuid;
};

// Struct to represent ESP32 Pipes
struct ESP32Pipes
{
    std::string handshake_rx;
    std::string handshake_tx;
    std::string message;
    std::string log;
    std::string config;

    // New fields for corresponding UUIDs
    std::string handshake_rx_uuid;
    std::string handshake_tx_uuid;
    std::string message_uuid;
    std::string log_uuid;
    std::string config_uuid;
};

#endif // BLETYPES_H

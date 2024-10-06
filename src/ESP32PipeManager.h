// src/ESP32PipeManager.h

#ifndef ESP32PIPEMANAGER_H
#define ESP32PIPEMANAGER_H

#include <string>
#include <map>
#include "BLETypes.h"

class ESP32PipeManager
{
public:
    ESP32PipeManager();
    ~ESP32PipeManager();

    // Initialize pipes based on UUID to Path mapping
    void initializePipes(const std::map<std::string, std::string> &uuidToPathMap);

    // Getter for ESP32 Pipes
    ESP32Pipes getESP32Pipes() const;

private:
    ESP32Pipes esp32Pipes;

    // ESP32 UUID Definitions
    const std::string UUID_HANDSHAKE_RX = "12345678-1234-5678-1234-56789abcdef4";
    const std::string UUID_HANDSHAKE_TX = "12345678-1234-5678-1234-56789abcdef5";
    const std::string UUID_MESSAGE = "12345678-1234-5678-1234-56789abcdef3";
    const std::string UUID_LOG = "12345678-1234-5678-1234-56789abcdef2";
    const std::string UUID_CONFIG = "12345678-1234-5678-1234-56789abcdef1";
    // Add more UUIDs as needed
};

#endif // ESP32PIPEMANAGER_H

// src/ESP32PipeManager.cpp

#include "ESP32PipeManager.h"
#include <iostream>
#include "Utils.h"

// Constructor
ESP32PipeManager::ESP32PipeManager()
{
    std::cout << "[ESP32PipeManager] Constructor called." << std::endl;
}

// Destructor
ESP32PipeManager::~ESP32PipeManager()
{
    std::cout << "[ESP32PipeManager] Destructor called." << std::endl;
}

// Initialize pipes based on UUID to Path mapping
void ESP32PipeManager::initializePipes(const std::map<std::string, std::string> &uuidToPathMap)
{
    std::cout << "[ESP32PipeManager] Initializing ESP32 Pipes based on UUID mappings." << std::endl;

    // Assign paths and UUIDs to ESP32Pipes if UUIDs are found
    std::string lowerUUID;

    lowerUUID = toLower(UUID_HANDSHAKE_RX);
    auto it = uuidToPathMap.find(lowerUUID);
    if (it != uuidToPathMap.end())
    {
        esp32Pipes.handshake_rx = it->second;
        esp32Pipes.handshake_rx_uuid = lowerUUID;
    }
    else
    {
        std::cerr << "[ESP32PipeManager] Warning: Handshake_RX UUID not found." << std::endl;
    }

    lowerUUID = toLower(UUID_HANDSHAKE_TX);
    it = uuidToPathMap.find(lowerUUID);
    if (it != uuidToPathMap.end())
    {
        esp32Pipes.handshake_tx = it->second;
        esp32Pipes.handshake_tx_uuid = lowerUUID;
    }
    else
    {
        std::cerr << "[ESP32PipeManager] Warning: Handshake_TX UUID not found." << std::endl;
    }

    lowerUUID = toLower(UUID_MESSAGE);
    it = uuidToPathMap.find(lowerUUID);
    if (it != uuidToPathMap.end())
    {
        esp32Pipes.message = it->second;
        esp32Pipes.message_uuid = lowerUUID;
    }
    else
    {
        std::cerr << "[ESP32PipeManager] Warning: Message UUID not found." << std::endl;
    }

    lowerUUID = toLower(UUID_LOG);
    it = uuidToPathMap.find(lowerUUID);
    if (it != uuidToPathMap.end())
    {
        esp32Pipes.log = it->second;
        esp32Pipes.log_uuid = lowerUUID;
    }
    else
    {
        std::cerr << "[ESP32PipeManager] Warning: Log UUID not found." << std::endl;
    }

    lowerUUID = toLower(UUID_CONFIG);
    it = uuidToPathMap.find(lowerUUID);
    if (it != uuidToPathMap.end())
    {
        esp32Pipes.config = it->second;
        esp32Pipes.config_uuid = lowerUUID;
    }
    else
    {
        std::cerr << "[ESP32PipeManager] Warning: Config UUID not found." << std::endl;
    }

    // Display ESP32 Pipes with corresponding UUIDs
    std::cout << "\n=== ESP32 Pipes Mapping ===" << std::endl;
    std::cout << "Handshake_RX Path: " << (esp32Pipes.handshake_rx.empty() ? "Not Found" : esp32Pipes.handshake_rx)
              << " | UUID: " << esp32Pipes.handshake_rx_uuid << std::endl;
    std::cout << "Handshake_TX Path: " << (esp32Pipes.handshake_tx.empty() ? "Not Found" : esp32Pipes.handshake_tx)
              << " | UUID: " << esp32Pipes.handshake_tx_uuid << std::endl;
    std::cout << "Message Path: " << (esp32Pipes.message.empty() ? "Not Found" : esp32Pipes.message)
              << " | UUID: " << esp32Pipes.message_uuid << std::endl;
    std::cout << "Log Path: " << (esp32Pipes.log.empty() ? "Not Found" : esp32Pipes.log)
              << " | UUID: " << esp32Pipes.log_uuid << std::endl;
    std::cout << "Config Path: " << (esp32Pipes.config.empty() ? "Not Found" : esp32Pipes.config)
              << " | UUID: " << esp32Pipes.config_uuid << std::endl;
    std::cout << "=== End of ESP32 Pipes Mapping ===" << std::endl;
}

// Getter for ESP32 Pipes
ESP32Pipes ESP32PipeManager::getESP32Pipes() const
{
    return esp32Pipes;
}

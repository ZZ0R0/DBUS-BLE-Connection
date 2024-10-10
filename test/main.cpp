// test/main.cpp

#include "BLEManager.h"
#include "DeviceManager.h"
#include "BLETypes.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

// ----------------------
// BLE UUID Definitions
// ----------------------
const std::string SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0";
const std::string CHARACTERISTIC_CONFIG_UUID = "12345678-1234-5678-1234-56789abcdef1";
const std::string CHARACTERISTIC_LOG_UUID = "12345678-1234-5678-1234-56789abcdef2";
const std::string CHARACTERISTIC_MESSAGE_UUID = "12345678-1234-5678-1234-56789abcdef3";
const std::string CHARACTERISTIC_HANDSHAKE_RX_UUID = "12345678-1234-5678-1234-56789abcdef4";
const std::string CHARACTERISTIC_HANDSHAKE_TX_UUID = "12345678-1234-5678-1234-56789abcdef5";

// ----------------------
// Function Definitions
// ----------------------

// Function to initialize BLE and connect to ESP32
bool initializeBLE(BLEManager &bleManager, const std::string &serviceUUID)
{
    std::cout << "Initializing BLE Manager..." << std::endl;
    if (!bleManager.initialize())
    {
        std::cerr << "Failed to initialize BLE Manager." << std::endl;
        return false;
    }

    std::cout << "Listing connected BLE devices..." << std::endl;
    std::vector<BluetoothDevice> devices = bleManager.listConnectedDevices();

    if (devices.empty())
    {
        std::cerr << "No connected BLE devices found." << std::endl;
        return false;
    }

    // Select the first connected device
    BluetoothDevice selectedDevice = devices.front();
    std::cout << "Selected Device: " << selectedDevice.name << " [" << selectedDevice.macAddress << "]" << std::endl;

    if (!bleManager.connectToDevice(selectedDevice.macAddress))
    {
        std::cerr << "Failed to connect to the BLE device." << std::endl;
        return false;
    }

    std::cout << "Listing all characteristics..." << std::endl;
    if (!bleManager.listAllCharacteristics())
    {
        std::cerr << "Failed to list characteristics." << std::endl;
        return false;
    }

    CharacteristicManager *charManager = bleManager.getCharacteristicManager();
    if (charManager)
    {
        std::map<std::string, std::string> uuidToPath = charManager->getUuidToPathMap();
        std::cout << "\n--- Discovered Characteristics ---" << std::endl;
        for (const auto &entry : uuidToPath)
        {
            std::cout << "UUID: " << entry.first << " | Path: " << entry.second << std::endl;
        }
        std::cout << "----------------------------------\n"
                  << std::endl;
    }
    else
    {
        std::cerr << "CharacteristicManager is not initialized." << std::endl;
        return false;
    }

    return true;
}

// Function to configure ESP32 radio settings
bool configureESP32(BLEManager &bleManager)
{
    std::cout << "Configuring ESP32 radio settings..." << std::endl;
    std::string configCommand = "FREQ:433.0;MOD:OOK;PWR:10;ROLE:Transmitter;";
    if (!bleManager.writeToPipe(CHARACTERISTIC_CONFIG_UUID, configCommand))
    {
        std::cerr << "Failed to write configuration to ESP32." << std::endl;
        return false;
    }
    std::cout << "Configuration command sent successfully." << std::endl;
    return true;
}

// Function to perform handshake with ESP32
bool performHandshake(BLEManager &bleManager)
{
    std::cout << "Performing handshake with ESP32..." << std::endl;
    std::string handshakeMessage = "Handshake_Request";

    if (!bleManager.writeToPipe(CHARACTERISTIC_HANDSHAKE_RX_UUID, handshakeMessage))
    {
        std::cerr << "Failed to send handshake message to ESP32." << std::endl;
        return false;
    }

    std::cout << "Handshake message sent. Waiting for response..." << std::endl;
    std::string response;
    bool received = false;
    for (int i = 0; i < 5; ++i)
    {
        if (bleManager.readFromPipe(CHARACTERISTIC_HANDSHAKE_TX_UUID, response))
        {
            std::cout << "Received Handshake Response: " << response << std::endl;
            received = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (!received)
    {
        std::cerr << "Handshake response not received within timeout." << std::endl;
        return false;
    }

    std::cout << "Handshake completed successfully." << std::endl;
    return true;
}

// Function to send a message to ESP32
bool sendMessage(BLEManager &bleManager, const std::string &message)
{
    std::cout << "Sending message to ESP32: " << message << std::endl;

    if (!bleManager.writeToPipe(CHARACTERISTIC_MESSAGE_UUID, message))
    {
        std::cerr << "Failed to send message to ESP32." << std::endl;
        return false;
    }

    std::cout << "Message sent successfully." << std::endl;
    return true;
}

// Function to handle incoming log data
void handleLogData(const std::string &log)
{
    std::cout << "Received Log Data: " << log << std::endl;
}

int main()
{
    BLEManager bleManager;

    // Step 1: Initialize BLE and connect to ESP32
    if (!initializeBLE(bleManager, SERVICE_UUID))
    {
        std::cerr << "BLE Initialization and connection failed." << std::endl;
        return 1;
    }

    // Step 2: Configure ESP32 radio settings
    if (!configureESP32(bleManager))
    {
        std::cerr << "ESP32 Configuration failed." << std::endl;
        return 1;
    }

    // Step 3: Perform handshake
    if (!performHandshake(bleManager))
    {
        std::cerr << "Handshake with ESP32 failed." << std::endl;
        return 1;
    }

    // Step 4: Send a test message
    std::string testMessage = "Hello from Computer!";
    if (!sendMessage(bleManager, testMessage))
    {
        std::cerr << "Failed to send test message." << std::endl;
        return 1;
    }

    // Step 5: Listen for incoming log data by polling
    std::cout << "BLE Communication setup complete. Listening for logs..." << std::endl;
    int logCount = 0;
    int maxLogs = 10; // Limit log entries

    while (logCount < maxLogs)
    {
        std::string logData;
        if (bleManager.readFromPipe(CHARACTERISTIC_LOG_UUID, logData))
        {
            handleLogData(logData);
            logCount++;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Received " << maxLogs << " logs. Exiting loop." << std::endl;
    return 0;
}

#include "BLEManager.h"
#include <iostream>
#include <limits>

int main()
{
    // Create BLEManager instance
    BLEManager bleManager;

    // Initialize BLEManager
    if (!bleManager.initialize())
    {
        std::cerr << "Failed to initialize BLE Manager." << std::endl;
        return 1;
    }

    // Try to list connected Bluetooth devices
    std::cout << "Listing connected Bluetooth devices..." << std::endl;
    std::vector<BluetoothDevice> devices = bleManager.listConnectedDevices();

    // Check if any devices were found
    if (devices.empty())
    {
        std::cerr << "No connected Bluetooth devices found." << std::endl;
        return 1;
    }

    // List the found devices (only once)
    std::cout << "Available Devices:" << std::endl;
    for (size_t i = 0; i < devices.size(); ++i)
    {
        std::cout << "  " << (i + 1) << ". " << devices[i].name << " (" << devices[i].path << ")" << std::endl;
    }

    // Prompt the user to select a device
    int choice = -1;
    while (choice < 1 || choice > static_cast<int>(devices.size()))
    {
        std::cout << "Select a device by entering the corresponding number: ";
        std::cin >> choice;

        // Ensure valid input
        if (std::cin.fail())
        {
            std::cin.clear();                                                   // Clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            choice = -1;
        }
    }

    // Get the selected device
    BluetoothDevice selectedDevice = devices[choice - 1];
    std::cout << "You selected: " << selectedDevice.name << " (" << selectedDevice.path << ")" << std::endl;

    // Set the selected device in BLEManager
    bleManager.setSelectedDevicePath(selectedDevice.path);

    // List all characteristics of the selected device
    if (!bleManager.listAllCharacteristics())
    {
        std::cerr << "Failed to list characteristics for the selected device." << std::endl;
        return 1;
    }

    // Initialize the ESP32 pipes based on the characteristics found
    if (!bleManager.performHandshake())
    {
        std::cerr << "Failed to perform handshake with the selected device." << std::endl;
        return 1;
    }

    std::string messageToSend;
    std::cout << "Enter a message to send to the device: ";
    std::cin.ignore(); // Clear the newline from previous input
    std::getline(std::cin, messageToSend);

    if (!bleManager.sendMessage(messageToSend))
    {
        std::cerr << "Failed to send message to the device." << std::endl;
        return 1;
    }

    // Wait for the device to respond
    std::string receivedMessage;
    if (bleManager.receiveMessage(receivedMessage))
    {
        std::cout << "Message received from the device: " << receivedMessage << std::endl;
    }
    else
    {
        std::cerr << "Failed to receive message from the device." << std::endl;
    }

    return 0;
}

#include "BLEManager.h"
#include "Utils.h"
#include "DbusConnection.h"
#include "CharacteristicManager.h"
#include "ESP32PipeManager.h"
#include "DeviceManager.h" // Add this line to include the DeviceManager class
#include <iostream>
#include <cstring> // Add this line to include the cstring header for strcmp

// Constructor: Initializes member variables
BLEManager::BLEManager()
    : dbusConn(nullptr), charManager(nullptr), pipeManager(nullptr), selectedDevicePath("")
{
    std::cout << "[BLEManager] Constructor called." << std::endl;
}

// Destructor: Cleans up allocated resources
BLEManager::~BLEManager()
{
    std::cout << "[BLEManager] Destructor called." << std::endl;
    if (pipeManager)
        delete pipeManager;
    if (charManager)
        delete charManager;
    if (dbusConn)
        delete dbusConn;
}

// Initialize BLE Manager
bool BLEManager::initialize()
{
    std::cout << "[BLEManager] Initializing BLE Manager..." << std::endl;

    // Initialize D-Bus connection
    dbusConn = new DbusConnection();
    if (!dbusConn->initialize())
    {
        std::cerr << "[BLEManager] Failed to initialize D-Bus connection." << std::endl;
        return false;
    }

    return true;
}

// Connect to a specific device
bool BLEManager::connectToDevice(const BluetoothDevice &device)
{
    std::cout << "[BLEManager] Starting handshake with device: " << device.name << " (" << device.path << ")" << std::endl;

    if (!charManager || !pipeManager)
    {
        std::cerr << "[BLEManager] CharacteristicManager or ESP32PipeManager is not initialized." << std::endl;
        return false;
    }

    // Perform the handshake using the ESP32 handshake characteristics
    ESP32Pipes pipes = pipeManager->getESP32Pipes();

    if (pipes.handshake_rx.empty() || pipes.handshake_tx.empty())
    {
        std::cerr << "[BLEManager] Handshake RX or TX paths are not initialized." << std::endl;
        return false;
    }

    // Send handshake message to ESP32 (to RX characteristic)
    std::string handshakeMessage = "Handshake_Request";
    std::cout << "[BLEManager] Sending handshake request to characteristic at: " << pipes.handshake_rx << std::endl;

    if (!charManager->writeCharacteristic(pipes.handshake_rx, handshakeMessage))
    {
        std::cerr << "[BLEManager] Failed to send handshake request." << std::endl;
        return false;
    }

    // Read the handshake response from ESP32 (from TX characteristic)
    std::string handshakeResponse;
    std::cout << "[BLEManager] Waiting for handshake response from characteristic at: " << pipes.handshake_tx << std::endl;

    if (!charManager->readCharacteristic(pipes.handshake_tx, handshakeResponse))
    {
        std::cerr << "[BLEManager] Failed to receive handshake response." << std::endl;
        return false;
    }

    std::cout << "[BLEManager] Handshake response received: " << handshakeResponse << std::endl;

    // Validate the response
    if (handshakeResponse == "Handshake_OK")
    {
        std::cout << "[BLEManager] Handshake successful." << std::endl;
        return true;
    }
    else
    {
        std::cerr << "[BLEManager] Handshake failed or invalid response." << std::endl;
        return false;
    }
}

// List all connected Bluetooth devices (this is the low-level function that interacts with D-Bus)
std::vector<BluetoothDevice> BLEManager::listConnectedDevices()
{
    std::vector<BluetoothDevice> devices;

    if (!dbusConn)
    {
        std::cerr << "[BLEManager] D-Bus connection is not initialized." << std::endl;
        return devices;
    }

    std::cout << "[BLEManager] Listing connected Bluetooth devices..." << std::endl;

    // Call the Introspect method to get available devices from the Adapter interface
    DBusMessage *msg = dbus_message_new_method_call(
        "org.bluez", "/org/bluez/hci0", "org.freedesktop.DBus.Introspectable", "Introspect");

    if (!msg)
    {
        std::cerr << "[BLEManager] Failed to create Introspect message." << std::endl;
        return devices;
    }

    DBusError error;
    dbus_error_init(&error);

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(dbusConn->getConnection(), msg, -1, &error);
    dbus_message_unref(msg);

    if (!reply)
    {
        std::cerr << "[BLEManager] Introspect call failed: " << error.message << std::endl;
        dbus_error_free(&error);
        return devices;
    }

    // Parse the Introspection XML
    DBusMessageIter args;
    if (!dbus_message_iter_init(reply, &args) || DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        std::cerr << "[BLEManager] Introspect reply is not a valid XML string." << std::endl;
        dbus_message_unref(reply);
        return devices;
    }

    char *xmlData;
    dbus_message_iter_get_basic(&args, &xmlData);
    std::string xmlString(xmlData);
    dbus_message_unref(reply);

    // Extract the child paths from the introspection XML (this should give us device paths)
    std::vector<std::string> devicePaths = extractChildPaths(xmlString, "/org/bluez/hci0");

    // Now check for connected devices and get their name by querying their properties
    for (const std::string &devicePath : devicePaths)
    {
        // Create a message to retrieve the "Connected" property
        DBusMessage *msg = dbus_message_new_method_call(
            "org.bluez", devicePath.c_str(), "org.freedesktop.DBus.Properties", "Get");

        if (!msg)
        {
            std::cerr << "[BLEManager] Failed to create Properties.Get message for " << devicePath << std::endl;
            continue;
        }

        const char *iface = "org.bluez.Device1";
        const char *prop_connected = "Connected";

        dbus_message_append_args(msg,
                                 DBUS_TYPE_STRING, &iface,
                                 DBUS_TYPE_STRING, &prop_connected,
                                 DBUS_TYPE_INVALID);

        DBusMessage *reply = dbus_connection_send_with_reply_and_block(dbusConn->getConnection(), msg, -1, &error);
        dbus_message_unref(msg);

        if (!reply)
        {
            if (dbus_error_is_set(&error))
            {
                std::cerr << "[BLEManager] Properties.Get (Connected) call failed for " << devicePath << ": " << error.message << std::endl;
                dbus_error_free(&error);
            }
            continue;
        }

        // Check if the device is connected
        DBusMessageIter replyIter;
        if (dbus_message_iter_init(reply, &replyIter) && DBUS_TYPE_VARIANT == dbus_message_iter_get_arg_type(&replyIter))
        {
            DBusMessageIter variantIter;
            dbus_message_iter_recurse(&replyIter, &variantIter);
            if (DBUS_TYPE_BOOLEAN == dbus_message_iter_get_arg_type(&variantIter))
            {
                dbus_bool_t connected;
                dbus_message_iter_get_basic(&variantIter, &connected);

                if (connected)
                {
                    // Add the connected device to the list
                    BluetoothDevice device;
                    device.path = devicePath;
                    device.connected = true;

                    // Retrieve the device's "Name" property
                    msg = dbus_message_new_method_call(
                        "org.bluez", devicePath.c_str(), "org.freedesktop.DBus.Properties", "Get");

                    if (msg)
                    {
                        const char *prop_name = "Name";
                        dbus_message_append_args(msg,
                                                 DBUS_TYPE_STRING, &iface,
                                                 DBUS_TYPE_STRING, &prop_name,
                                                 DBUS_TYPE_INVALID);

                        DBusMessage *name_reply = dbus_connection_send_with_reply_and_block(dbusConn->getConnection(), msg, -1, &error);
                        dbus_message_unref(msg);

                        if (name_reply)
                        {
                            DBusMessageIter name_reply_iter;
                            if (dbus_message_iter_init(name_reply, &name_reply_iter) &&
                                DBUS_TYPE_VARIANT == dbus_message_iter_get_arg_type(&name_reply_iter))
                            {
                                DBusMessageIter name_variant_iter;
                                dbus_message_iter_recurse(&name_reply_iter, &name_variant_iter);

                                if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&name_variant_iter))
                                {
                                    char *name;
                                    dbus_message_iter_get_basic(&name_variant_iter, &name);
                                    device.name = std::string(name);
                                }
                            }
                            dbus_message_unref(name_reply);
                        }
                    }

                    devices.push_back(device);
                }
            }
        }

        dbus_message_unref(reply);
    }

    return devices;
}

// Select a device (for simplicity, selecting the first connected device)
bool BLEManager::selectDevice()
{
    std::cout << "[BLEManager] Selecting a device..." << std::endl;

    std::vector<BluetoothDevice> devices = listConnectedDevices();
    if (devices.empty())
    {
        std::cerr << "[BLEManager] No connected Bluetooth devices found." << std::endl;
        return false;
    }

    BluetoothDevice device = devices.front();
    selectedDevicePath = device.path;
    std::cout << "[BLEManager] Selected Device: " << device.name << " (" << device.path << ")" << std::endl;
    return true;
}

// List all characteristics of the selected device
bool BLEManager::listAllCharacteristics()
{
    if (selectedDevicePath.empty())
    {
        std::cerr << "[BLEManager] No device selected to list characteristics." << std::endl;
        return false;
    }

    std::cout << "[BLEManager] Listing all characteristics for device: " << selectedDevicePath << std::endl;

    charManager = new CharacteristicManager(*dbusConn, selectedDevicePath);
    if (!charManager->listAllCharacteristics())
    {
        std::cerr << "[BLEManager] Failed to list characteristics." << std::endl;
        return false;
    }

    pipeManager = new ESP32PipeManager();
    pipeManager->initializePipes(charManager->getUuidToPathMap());
    return true;
}

// Perform handshake with the device
bool BLEManager::performHandshake()
{
    std::cout << "[BLEManager] Performing handshake..." << std::endl;

    if (!charManager || !pipeManager)
    {
        std::cerr << "[BLEManager] CharacteristicManager or ESP32PipeManager is not initialized." << std::endl;
        return false;
    }

    ESP32Pipes pipes = pipeManager->getESP32Pipes();

    // Ensure that both RX and TX paths are available
    if (pipes.handshake_rx.empty() || pipes.handshake_tx.empty())
    {
        std::cerr << "[BLEManager] Handshake paths are not properly initialized." << std::endl;
        return false;
    }

    // Write a handshake request to handshake_rx
    std::string handshakeRequest = "Handshake_Request";
    std::cout << "[BLEManager] Writing to characteristic at path: " << pipes.handshake_rx << " | Value: " << handshakeRequest << std::endl;
    if (!charManager->writeCharacteristic(pipes.handshake_rx, handshakeRequest))
    {
        std::cerr << "[BLEManager] Failed to write handshake request." << std::endl;
        return false;
    }

    std::cout << "[BLEManager] Handshake request sent. Awaiting response..." << std::endl;

    // Read handshake response from handshake_tx
    std::string handshakeResponse;
    std::cout << "[BLEManager] Reading from characteristic at path: " << pipes.handshake_tx << std::endl;
    if (!charManager->readCharacteristic(pipes.handshake_tx, handshakeResponse))
    {
        std::cerr << "[BLEManager] Failed to read handshake response." << std::endl;
        return false;
    }

    std::cout << "[BLEManager] Handshake response received: " << handshakeResponse << std::endl;

    if (handshakeResponse == "Handshake_OK")
    {
        std::cout << "[BLEManager] Handshake successful." << std::endl;
        return true;
    }
    else
    {
        std::cerr << "[BLEManager] Handshake failed or invalid response." << std::endl;
        return false;
    }
}

// Send a message to the device
bool BLEManager::sendMessage(const std::string &message)
{
    std::cout << "[BLEManager] Sending message: " << message << std::endl;

    if (!charManager || !pipeManager)
    {
        std::cerr << "[BLEManager] CharacteristicManager or ESP32PipeManager is not initialized." << std::endl;
        return false;
    }

    ESP32Pipes pipes = pipeManager->getESP32Pipes();

    if (pipes.message.empty())
    {
        std::cerr << "[BLEManager] Message characteristic path is not set." << std::endl;
        return false;
    }

    std::cout << "[BLEManager] Writing to characteristic at path: " << pipes.message << " | Value: " << message << std::endl;
    if (!charManager->writeCharacteristic(pipes.message, message))
    {
        std::cerr << "[BLEManager] Failed to write message." << std::endl;
        return false;
    }

    std::cout << "[BLEManager] Message sent successfully." << std::endl;
    return true;
}

// Receive a message from the device
bool BLEManager::receiveMessage(std::string &message)
{
    std::cout << "[BLEManager] Receiving message from device..." << std::endl;

    if (!charManager || !pipeManager)
    {
        std::cerr << "[BLEManager] CharacteristicManager or ESP32PipeManager is not initialized." << std::endl;
        return false;
    }

    ESP32Pipes pipes = pipeManager->getESP32Pipes();

    if (pipes.message.empty())
    {
        std::cerr << "[BLEManager] Message characteristic path is not set." << std::endl;
        return false;
    }

    std::cout << "[BLEManager] Reading from characteristic at path: " << pipes.message << std::endl;
    if (!charManager->readCharacteristic(pipes.message, message))
    {
        std::cerr << "[BLEManager] Failed to read message." << std::endl;
        return false;
    }

    std::cout << "[BLEManager] Received message: " << message << std::endl;
    return true;
}

// Recursively print the D-Bus object tree
bool BLEManager::printObjectTree(const std::string &objectPath, int indent)
{
    // Implementation can be delegated to another utility or manager if needed
    // For simplicity, we can leave it as a placeholder
    std::cout << "[BLEManager] printObjectTree not implemented." << std::endl;
    return false;
}

// Getter for selectedDevicePath
std::string BLEManager::getSelectedDevicePath() const
{
    return selectedDevicePath;
}

// Setter for selectedDevicePath
void BLEManager::setSelectedDevicePath(const std::string &devicePath)
{
    selectedDevicePath = devicePath;
}

// Getter for ESP32 Pipes
ESP32Pipes BLEManager::getESP32Pipes() const
{
    if (!pipeManager)
    {
        return ESP32Pipes();
    }
    return pipeManager->getESP32Pipes();
}

// Function to list available methods and interfaces
void BLEManager::listAvailableMethods(const std::string &objectPath)
{
    std::cout << "[BLEManager] Listing available methods and interfaces for object path: " << objectPath << std::endl;

    DBusMessage *msg = dbus_message_new_method_call(
        "org.freedesktop.DBus",
        objectPath.c_str(),
        "org.freedesktop.DBus.Introspectable",
        "Introspect");

    if (!msg)
    {
        std::cerr << "[BLEManager] Failed to create introspection message for path: " << objectPath << std::endl;
        return;
    }

    DBusError err;
    dbus_error_init(&err);
    DBusMessage *reply = dbus_connection_send_with_reply_and_block(dbusConn->getConnection(), msg, -1, &err);

    if (!reply)
    {
        if (dbus_error_is_set(&err))
        {
            std::cerr << "[BLEManager] Introspect call failed: " << err.message << std::endl;
            dbus_error_free(&err);
        }
        else
        {
            std::cerr << "[BLEManager] Introspect call failed: Unknown error" << std::endl;
        }
        dbus_message_unref(msg);
        return;
    }

    // Parse the XML reply to list available methods
    DBusMessageIter args;
    if (!dbus_message_iter_init(reply, &args))
    {
        std::cerr << "[BLEManager] Introspect reply has no arguments." << std::endl;
    }
    else if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&args))
    {
        const char *xmlData;
        dbus_message_iter_get_basic(&args, &xmlData);
        std::cout << "[BLEManager] Introspect XML: \n"
                  << xmlData << std::endl;
        // Process XML data to extract and display interfaces and methods
    }

    dbus_message_unref(reply);
}
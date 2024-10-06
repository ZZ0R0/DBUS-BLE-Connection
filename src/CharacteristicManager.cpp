#include "CharacteristicManager.h"
#include "DbusConnection.h"
#include "Utils.h"
#include <iostream>

CharacteristicManager::CharacteristicManager(DbusConnection &dbusConn, const std::string &devicePath_)
    : dbusConnection(dbusConn), devicePath(devicePath_)
{
    std::cout << "[CharacteristicManager] Constructor called." << std::endl;
}

CharacteristicManager::~CharacteristicManager()
{
    std::cout << "[CharacteristicManager] Destructor called." << std::endl;
}

// Getter for UUID to Path map
std::map<std::string, std::string> CharacteristicManager::getUuidToPathMap() const
{
    return uuidToPathMap;
}

// List all characteristics and populate uuidToPathMap
bool CharacteristicManager::listAllCharacteristics()
{
    std::cout << "[CharacteristicManager] Listing all characteristics for device: " << devicePath << std::endl;

    // Introspect the device to find services
    DBusMessage *msg_device = dbus_message_new_method_call(
        "org.bluez",
        devicePath.c_str(),
        "org.freedesktop.DBus.Introspectable",
        "Introspect");

    if (!msg_device)
    {
        std::cerr << "[CharacteristicManager] Failed to create Introspect message for device path: " << devicePath << "." << std::endl;
        return false;
    }

    DBusError err_device;
    dbus_error_init(&err_device);

    DBusMessage *reply_device = dbus_connection_send_with_reply_and_block(dbusConnection.getConnection(), msg_device, -1, &err_device);
    dbus_message_unref(msg_device);

    if (!reply_device)
    {
        if (dbus_error_is_set(&err_device))
        {
            std::cerr << "[CharacteristicManager] Introspect call failed for device path " << devicePath << ": " << err_device.message << std::endl;
            dbus_error_free(&err_device);
        }
        else
        {
            std::cerr << "[CharacteristicManager] Introspect call failed for device path " << devicePath << ": Unknown error." << std::endl;
        }
        return false;
    }

    // Read the introspection XML
    DBusMessageIter args_device;
    if (!dbus_message_iter_init(reply_device, &args_device))
    {
        std::cerr << "[CharacteristicManager] Introspect reply has no arguments for device path " << devicePath << "." << std::endl;
        dbus_message_unref(reply_device);
        return false;
    }

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args_device))
    {
        std::cerr << "[CharacteristicManager] Introspect reply is not a string for device path " << devicePath << "." << std::endl;
        dbus_message_unref(reply_device);
        return false;
    }

    char *xmlData;
    dbus_message_iter_get_basic(&args_device, &xmlData);
    std::string xmlString(xmlData);

    dbus_message_unref(reply_device);

    // Extract service paths
    std::vector<std::string> servicePaths = extractChildPaths(xmlString, devicePath);

    std::cout << "[CharacteristicManager] Found " << servicePaths.size() << " service(s) under " << devicePath << "." << std::endl;

    // Iterate through each service to find characteristics
    for (const auto &servicePath : servicePaths)
    {
        // Introspect each service to find characteristics
        DBusMessage *msg_service = dbus_message_new_method_call(
            "org.bluez",
            servicePath.c_str(),
            "org.freedesktop.DBus.Introspectable",
            "Introspect");

        if (!msg_service)
        {
            std::cerr << "[CharacteristicManager] Failed to create Introspect message for service path: " << servicePath << "." << std::endl;
            continue;
        }

        DBusError err_service;
        dbus_error_init(&err_service);

        DBusMessage *reply_service = dbus_connection_send_with_reply_and_block(dbusConnection.getConnection(), msg_service, -1, &err_service);
        dbus_message_unref(msg_service);

        if (!reply_service)
        {
            if (dbus_error_is_set(&err_service))
            {
                std::cerr << "[CharacteristicManager] Introspect call failed for service path " << servicePath << ": " << err_service.message << std::endl;
                dbus_error_free(&err_service);
            }
            else
            {
                std::cerr << "[CharacteristicManager] Introspect call failed for service path " << servicePath << ": Unknown error." << std::endl;
            }
            continue;
        }

        // Read the introspection XML
        DBusMessageIter args_service;
        if (!dbus_message_iter_init(reply_service, &args_service))
        {
            std::cerr << "[CharacteristicManager] Introspect reply has no arguments for service path " << servicePath << "." << std::endl;
            dbus_message_unref(reply_service);
            continue;
        }

        if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args_service))
        {
            std::cerr << "[CharacteristicManager] Introspect reply is not a string for service path " << servicePath << "." << std::endl;
            dbus_message_unref(reply_service);
            continue;
        }

        char *xmlServiceData;
        dbus_message_iter_get_basic(&args_service, &xmlServiceData);
        std::string xmlServiceString(xmlServiceData);

        dbus_message_unref(reply_service);

        // Extract characteristic paths
        std::vector<std::string> charPaths = extractChildPaths(xmlServiceString, servicePath);

        std::cout << "[CharacteristicManager] Found " << charPaths.size() << " characteristic(s) under " << servicePath << "." << std::endl;

        // Iterate through each characteristic to gather information
        for (const auto &charPath : charPaths)
        {
            // Get the UUID property of the characteristic
            DBusMessage *msg_uuid = dbus_message_new_method_call(
                "org.bluez",
                charPath.c_str(),
                "org.freedesktop.DBus.Properties",
                "Get");

            if (!msg_uuid)
            {
                std::cerr << "[CharacteristicManager] Failed to create Properties.Get message for UUID at " << charPath << "." << std::endl;
                continue;
            }

            const char *iface_uuid = "org.bluez.GattCharacteristic1";
            const char *prop_uuid = "UUID";

            dbus_message_append_args(msg_uuid,
                                     DBUS_TYPE_STRING, &iface_uuid,
                                     DBUS_TYPE_STRING, &prop_uuid,
                                     DBUS_TYPE_INVALID);

            DBusError err_uuid;
            dbus_error_init(&err_uuid);

            DBusMessage *reply_uuid = dbus_connection_send_with_reply_and_block(dbusConnection.getConnection(), msg_uuid, -1, &err_uuid);
            dbus_message_unref(msg_uuid);

            if (!reply_uuid)
            {
                if (dbus_error_is_set(&err_uuid))
                {
                    std::cerr << "[CharacteristicManager] Properties.Get (UUID) call failed for " << charPath << ": " << err_uuid.message << std::endl;
                    dbus_error_free(&err_uuid);
                }
                else
                {
                    std::cerr << "[CharacteristicManager] Properties.Get (UUID) call failed for " << charPath << ": Unknown error." << std::endl;
                }
                continue;
            }

            // Read the UUID
            DBusMessageIter args_uuid;
            if (dbus_message_iter_init(reply_uuid, &args_uuid))
            {
                if (DBUS_TYPE_VARIANT == dbus_message_iter_get_arg_type(&args_uuid))
                {
                    DBusMessageIter variantIter;
                    dbus_message_iter_recurse(&args_uuid, &variantIter);
                    if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&variantIter))
                    {
                        char *uuidCStr;
                        dbus_message_iter_get_basic(&variantIter, &uuidCStr);
                        std::string charUUID(uuidCStr);

                        // Convert UUID to lowercase for consistent mapping
                        std::string lowerUUID = toLower(charUUID);

                        // Store the mapping
                        uuidToPathMap[lowerUUID] = charPath;

                        // Optionally, store in characteristics vector if needed elsewhere
                        BLECharacteristic characteristic;
                        characteristic.path = charPath;
                        characteristic.uuid = lowerUUID;
                        // characteristics.push_back(characteristic); // Not used here
                    }
                }
            }

            dbus_message_unref(reply_uuid);
        }
    }

    return true;
}

// Write to a characteristic
bool CharacteristicManager::writeCharacteristic(const std::string &charPath, const std::string &value)
{
    DBusMessage *msg = dbus_message_new_method_call(
        "org.bluez",
        charPath.c_str(),
        "org.bluez.GattCharacteristic1",
        "WriteValue");

    if (!msg)
    {
        std::cerr << "[CharacteristicManager] Failed to create WriteValue message for path: " << charPath << "." << std::endl;
        return false;
    }

    DBusMessageIter args;
    dbus_message_iter_init_append(msg, &args);

    // Use DBUS_TYPE_ARRAY with 'y' (byte) as the type to append a string as a byte array
    DBusMessageIter arrayIter;
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "y", &arrayIter);
    for (const char &byte : value)
    {
        unsigned char byteData = static_cast<unsigned char>(byte);
        dbus_message_iter_append_basic(&arrayIter, DBUS_TYPE_BYTE, &byteData);
    }
    dbus_message_iter_close_container(&args, &arrayIter);

    // Empty dictionary (for write options)
    DBusMessageIter dictIter;
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &dictIter);
    dbus_message_iter_close_container(&args, &dictIter);

    DBusError err;
    dbus_error_init(&err);

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(dbusConnection.getConnection(), msg, -1, &err);
    dbus_message_unref(msg);

    if (!reply)
    {
        if (dbus_error_is_set(&err))
        {
            std::cerr << "[CharacteristicManager] WriteValue call failed for " << charPath << ": " << err.message << std::endl;
            dbus_error_free(&err);
        }
        else
        {
            std::cerr << "[CharacteristicManager] WriteValue call failed for " << charPath << ": Unknown error." << std::endl;
        }
        return false;
    }

    dbus_message_unref(reply);
    return true;
}

// Read from a characteristic
bool CharacteristicManager::readCharacteristic(const std::string &charPath, std::string &value)
{
    DBusMessage *msg = dbus_message_new_method_call(
        "org.bluez",
        charPath.c_str(),
        "org.bluez.GattCharacteristic1",
        "ReadValue");

    if (!msg)
    {
        std::cerr << "[CharacteristicManager] Failed to create ReadValue message for path: " << charPath << "." << std::endl;
        return false;
    }

    // Empty dictionary for options
    DBusMessageIter args;
    dbus_message_iter_init_append(msg, &args);
    DBusMessageIter dictIter;
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &dictIter);
    dbus_message_iter_close_container(&args, &dictIter);

    DBusError err;
    dbus_error_init(&err);

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(dbusConnection.getConnection(), msg, -1, &err);
    dbus_message_unref(msg);

    if (!reply)
    {
        if (dbus_error_is_set(&err))
        {
            std::cerr << "[CharacteristicManager] ReadValue call failed for " << charPath << ": " << err.message << std::endl;
            dbus_error_free(&err);
        }
        else
        {
            std::cerr << "[CharacteristicManager] ReadValue call failed for " << charPath << ": Unknown error." << std::endl;
        }
        return false;
    }

    // Read the byte array from the reply
    DBusMessageIter iter;
    if (!dbus_message_iter_init(reply, &iter))
    {
        std::cerr << "[CharacteristicManager] ReadValue reply has no arguments for path " << charPath << "." << std::endl;
        dbus_message_unref(reply);
        return false;
    }

    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&iter))
    {
        std::cerr << "[CharacteristicManager] ReadValue reply is not an array for path " << charPath << "." << std::endl;
        dbus_message_unref(reply);
        return false;
    }

    DBusMessageIter arrayIter;
    dbus_message_iter_recurse(&iter, &arrayIter);

    std::string receivedValue;
    while (dbus_message_iter_get_arg_type(&arrayIter) != DBUS_TYPE_INVALID)
    {
        if (dbus_message_iter_get_arg_type(&arrayIter) == DBUS_TYPE_BYTE)
        {
            unsigned char byte;
            dbus_message_iter_get_basic(&arrayIter, &byte);
            receivedValue.push_back(static_cast<char>(byte));
        }
        dbus_message_iter_next(&arrayIter);
    }

    // Set the received value
    value = receivedValue;

    dbus_message_unref(reply);
    return true;
}
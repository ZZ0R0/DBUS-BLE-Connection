#include "DbusConnection.h"
#include <iostream>

// Constructor: Initializes member variables
DbusConnection::DbusConnection() : connection(nullptr)
{
    std::cout << "[DbusConnection] Constructor called." << std::endl;
}

// Destructor: Cleans up D-Bus connection
DbusConnection::~DbusConnection()
{
    if (connection)
    {
        // Remove the call to dbus_connection_close(), as it should not be called on shared connections.
        std::cout << "[DbusConnection] Unreferencing D-Bus connection." << std::endl;
        dbus_connection_unref(connection); // Unreference the connection, D-Bus will handle the cleanup.
        connection = nullptr;
    }
}

// Initialize D-Bus connection
bool DbusConnection::initialize()
{
    DBusError error;
    dbus_error_init(&error);

    // Connect to the system bus
    connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    if (dbus_error_is_set(&error))
    {
        std::cerr << "[DbusConnection] Connection Error: " << error.message << std::endl;
        dbus_error_free(&error);
        return false;
    }

    if (!connection)
    {
        std::cerr << "[DbusConnection] Failed to connect to the system bus." << std::endl;
        return false;
    }

    return true;
}

// Create a method call message
DBusMessage *DbusConnection::createMethodCall(const std::string &busName,
                                              const std::string &objectPath,
                                              const std::string &interfaceName,
                                              const std::string &methodName)
{
    return dbus_message_new_method_call(busName.c_str(), objectPath.c_str(),
                                        interfaceName.c_str(), methodName.c_str());
}

// Send message and block until a reply is received
DBusMessage *DbusConnection::sendAndBlock(DBusMessage *msg)
{
    DBusError error;
    dbus_error_init(&error);

    // Send message and block until a reply is received
    DBusMessage *reply = dbus_connection_send_with_reply_and_block(connection, msg, -1, &error);

    if (dbus_error_is_set(&error))
    {
        std::cerr << "[DbusConnection] Error in sendAndBlock: " << error.message << std::endl;
        dbus_error_free(&error);
        return nullptr;
    }

    return reply;
}

// Getter for the DBusConnection
DBusConnection *DbusConnection::getConnection() const
{
    return connection;
}

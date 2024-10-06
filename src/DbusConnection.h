#ifndef DBUSCONNECTION_H
#define DBUSCONNECTION_H

#include <dbus/dbus.h>
#include <string>

class DbusConnection
{
public:
    DbusConnection();
    ~DbusConnection();

    bool initialize();
    DBusConnection *getConnection() const;

    // New method declarations
    DBusMessage *createMethodCall(const std::string &busName,
                                  const std::string &objectPath,
                                  const std::string &interfaceName,
                                  const std::string &methodName);

    DBusMessage *sendAndBlock(DBusMessage *msg);

private:
    DBusConnection *connection;
};

#endif // DBUSCONNECTION_H

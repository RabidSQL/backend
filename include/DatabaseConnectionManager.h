#ifndef RABIDSQL_DATABASECONNECTIONMANAGER_H
#define RABIDSQL_DATABASECONNECTIONMANAGER_H

#include "Variant.h"

#include <map>

namespace RabidSQL {

class ConnectionSettings;
class DatabaseConnection;
class DatabaseConnectionManager: virtual public SmartObject
{
    friend class DatabaseConnection;
    static const unsigned int DEFAULT_EXPIRY = 10;

public:

    DatabaseConnectionManager(DatabaseConnection *mainConnection,
                              ConnectionSettings *settings);
    std::string reserveDatabaseConnection(int expiry = 0,
                                          SmartObject *receiver = nullptr);
    void releaseDatabaseConnection(std::string uuid);
    void call(std::string uuid, Variant uid, QueryEvent event,
        VariantVector arguments = VariantVector(), bool blocking = false);
    void killQuery(std::string uuid);
    ConnectionType getType();
    ~DatabaseConnectionManager();

    void disconnected(const VariantVector &args);

private:

    void call(DatabaseConnection *connection, Variant uid, QueryEvent event,
              VariantVector arguments= VariantVector());
    DatabaseConnection *getDatabaseConnection(std::string uuid);
    DatabaseConnection *reserveDatabaseConnectionObj(
            int timeout = 0, SmartObject *receiver = nullptr);

    ConnectionType type;
    DatabaseConnection *mainConnection;

    struct ConnectionRecord {
        std::string uuid;
        long expiry = 0;
        SmartObject *receiver = nullptr;
    };

    typedef std::map<DatabaseConnection *, ConnectionRecord> Connections;
    typedef std::map<std::string, DatabaseConnection *> DisconnectingConnections;

    Connections connections;
    DisconnectingConnections disconnectingConnections;
    unsigned int maxConnections;
};

} // namespace RabidSQL

#endif // RABIDSQL_DATABASECONNECTIONMANAGER_H

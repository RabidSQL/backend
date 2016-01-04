#ifndef RABIDSQL_DATABASECONNECTIONFACTORY_H
#define RABIDSQL_DATABASECONNECTIONFACTORY_H

#include "SettingsField.h"

namespace RabidSQL {

class ConnectionSettings;
class DatabaseConnection;
class DatabaseConnectionManager;
class DatabaseConnectionFactory
{
public:
    static DatabaseConnection *makeConnection(ConnectionSettings *settings);
    static DatabaseConnectionManager *makeManager(ConnectionSettings *settings);
    static std::vector<SettingsField> getSettingsFields(ConnectionSettings *settings);
    static std::map<ConnectionType, std::string> getTypeMap();
};

} // namespace RabidSQL

#endif // RABIDSQL_DATABASECONNECTIONFACTORY_H

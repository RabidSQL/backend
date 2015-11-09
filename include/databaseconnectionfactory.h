#ifndef RABIDSQL_DATABASECONNECTIONFACTORY_H
#define RABIDSQL_DATABASECONNECTIONFACTORY_H

#include "connectionsettings.h"
#include "databaseconnectionmanager.h"

namespace RabidSQL {

class QueryWidget;
class DatabaseConnectionFactory
{
public:
    static DatabaseConnection *makeConnection(ConnectionSettings *settings);
    static DatabaseConnectionManager *makeManager(ConnectionSettings *settings);
    static std::vector<SettingsField> getSettingsFields(ConnectionType type);
    static std::map<ConnectionType, std::string> getTypeMap();
};

} // namespace RabidSQL

#endif // RABIDSQL_DATABASECONNECTIONFACTORY_H

#include "app.h"
#include "databaseconnectionfactory.h"
#include "databaseconnectionmanager.h"
#include "mysql/include/databaseconnection.h"

namespace RabidSQL {

/**
 *
 * Makes a database connection
 *
 * @param settings The settings to use
 * @return The newly created database connection
 */
DatabaseConnection *DatabaseConnectionFactory::makeConnection(
        ConnectionSettings *settings)
{
    switch (settings->getType()) {
    case ConnectionSettings::MYSQL:
    default:
        return new MySQLDriver::DatabaseConnection(settings);
    }
}

/**
 *
 * Makes a connection manager for accessing a specific type of database
 *
 * @param settings The settings to use
 * @return A valid connection manager
 */
DatabaseConnectionManager *DatabaseConnectionFactory::makeManager(
        ConnectionSettings *settings)
{
    return new DatabaseConnectionManager(makeConnection(settings), settings);
}

/**
 *
 * Returns a vector of Settings Fields for the given connection type
 *
 * @param type The type of database connection to get settings for
 * @param settings The settings for the connection, if applicable
 * @return A QWidget to be injected into the settings form
 */
std::vector<SettingsField> DatabaseConnectionFactory::getSettingsFields(
        ConnectionSettings::type type)
{
    switch (type) {
    case ConnectionSettings::MYSQL:
    default:
        return MySQLDriver::DatabaseConnection::getSettingsFields();
    }
}

/**
 *
 * Returns possible database types as a map of type => textual type
 *
 * @return The map
 */
std::map<ConnectionSettings::type, std::string> DatabaseConnectionFactory::getTypeMap()
{
    std::map<ConnectionSettings::type, std::string> map;

    map[ConnectionSettings::INHERIT] = "Inherit";
    map[ConnectionSettings::MYSQL] = "MySQL";

    return map;
}

} // namespace RabidSQL

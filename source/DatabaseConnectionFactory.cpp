#include "App.h"
#include "ConnectionSettings.h"
#include "DatabaseConnectionFactory.h"
#include "DatabaseConnectionManager.h"
#include "DatabaseConnection.h"
#include "drivers/mysql/include/DatabaseConnection.h"

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
    case MYSQL:
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
 * @param settings The settings for the connection, if applicable
 * @return A QWidget to be injected into the settings form
 */
std::vector<SettingsField> DatabaseConnectionFactory::getSettingsFields(
        ConnectionSettings *settings)
{
    switch (settings->get("type").toUInt()) {
    case MYSQL:
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
std::map<ConnectionType, std::string> DatabaseConnectionFactory::getTypeMap()
{
    std::map<ConnectionType, std::string> map;

    map[INHERIT] = "Inherit";
    map[MYSQL] = "MySQL";

    return map;
}

} // namespace RabidSQL

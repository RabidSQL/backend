#ifndef RABIDSQL_MYSQLDRIVER_MYSQLDATABASECONNECTION_H
#define RABIDSQL_MYSQLDRIVER_MYSQLDATABASECONNECTION_H

#include "../../databaseconnection.h"
#include "connectionsettings.h"

namespace sql {
class Connection;
class Driver;
}

namespace RabidSQL {
namespace MySQLDriver {

class DatabaseConnection : virtual public RabidSQL::DatabaseConnection
{
public:
    DatabaseConnection(ConnectionSettings *settings);
    DatabaseConnection(DatabaseConnection *mainConnection,
                       DatabaseConnectionManager *manager);
    DatabaseConnection *clone(DatabaseConnectionManager *manager);
    static std::vector<SettingsField> getSettingsFields();

    QueryResult connect();
    QueryResult execute(VariantVector arguments);
    QueryResult getDatabases(
            std::vector<std::string> filter = std::vector<std::string>());
    QueryResult getTables(std::string database);
    QueryResult selectDatabase(std::string database);
    QueryResult killQuery(std::string uuid);
    void disconnect();
    virtual ~DatabaseConnection();

protected:
    int connection_id;

private:
    sql::Driver *driver;
    sql::Connection *connection;
    std::string hostname;
    std::string username;
    std::string password;
    unsigned int port;
};

} // namespace MySQLDriver
} // namespace RabidSQL

#endif // RABIDSQL_MYSQLDRIVER_MYSQLDATABASECONNECTION_H

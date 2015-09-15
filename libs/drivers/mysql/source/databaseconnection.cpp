#include "app.h"
#include "logic/include/structs.h"
#include "../include/databaseconnection.h"

#include <driver.h>
#include <resultset.h>
#include <statement.h>
#include <prepared_statement.h>

using namespace sql;

namespace RabidSQL {
namespace MySQLDriver {

/**
 *
 * Constructs the database connection
 *
 * @param settings The settings to use for constructing this connection
 */
DatabaseConnection::DatabaseConnection(ConnectionSettings *settings):
    RabidSQL::DatabaseConnection(settings)
{
    connection = nullptr;
    driver = nullptr;
    connection_id = 0;

    hostname = settings->get("hostname").toString();
    username = settings->get("username").toString();
    port = settings->get("port").toUInt();
    password = settings->get("password").toString();
}

/**
 *
 * Constructs a new database connection based on an existing one
 *
 * @param mainConnection The main connection we're using, if applicable. Used
 * by the DatabaseConnectionManager
 * @param manager The manager to use
 */
DatabaseConnection::DatabaseConnection(DatabaseConnection *mainConnection,
                                       DatabaseConnectionManager *manager):
    RabidSQL::DatabaseConnection::DatabaseConnection(mainConnection, manager)
{
    connection = nullptr;
    driver = nullptr;
    connection_id = 0;

    hostname = mainConnection->hostname;
    username = mainConnection->username;
    port = mainConnection->port;
    password = mainConnection->password;
}

/**
 *
 * Connects to the database
 *
 * Note that this expects that the database driver has setup all the things in
 * the connection.
 *
 * @return A QueryResult. error.isError will be false on success.
 */
QueryResult DatabaseConnection::connect()
{
    QueryResult result;

    if (driver == nullptr) {

        // Get driver instance
        driver = get_driver_instance();

        // Init SQL thread
        driver->threadInit();
    }

    if (connection == nullptr) {

        try {

            connection = driver->connect(hostname, username,
                                                        password);
        } catch (SQLException &e) {

            result.error.isError = true;
            result.error.code = e.getErrorCode();
            result.error.string = e.getSQLState() + ": " + e.what();

            return result;
        }

        // Reset result
        result = QueryResult();

        // Get connection id
        result = this->execute(VariantList() << "SELECT CONNECTION_ID()");
        if (result.rows.size() == 0 || result.error.isError) {

            // No row returned
            return result;
        }
        connection_id = result.rows.front().front().toUInt();

        // Reset result
        result = QueryResult();

        return result;
    }

    // Successful connection?
    return result;
}

/**
 *
 * Kills the query being executed by the given connection
 *
 * @param uuid the UUID of the connection to kill
 * @return the result of the kill query
 */
QueryResult DatabaseConnection::killQuery(std::string uuid)
{
    DatabaseConnection *connection;

    connection = dynamic_cast<DatabaseConnection *>(
                getDatabaseConnection(uuid));

    return execute(VariantList() << "KILL QUERY "
                   + Variant(connection->connection_id).toString());
}

/**
 *
 * Makes a new connection of this type
 *
 * @param manager The manager that is managing this connection set
 * @return the new connection
 */
DatabaseConnection *DatabaseConnection::clone(
        DatabaseConnectionManager *manager)
{
    return new DatabaseConnection(this, manager);
}

/**
 *
 * Retrieves databases and returns the results
 *
 * @param filter A string vector of databases to retrieve
 * @return The results of the query
 */
QueryResult DatabaseConnection::getDatabases(std::vector<std::string> filter)
{

    // Execute query
    VariantList arguments;

    if (filter.size() > 0) {

        std::string query = "SHOW DATABASES WHERE `Database` IN (";

        for (long i = filter.size() - 1; i >= 0; i--) {

            query += "?";

            if (i != 0) {

                query += ", ";
            }
        }

        query += ")";

        arguments << query;

        for (auto it = filter.begin(); it != filter.end(); ++it) {

            // Add to collection
            arguments << *it;
        }
    } else {

        arguments << "SHOW DATABASES";
    }

    return execute(arguments);
}

/**
 *
 * Retrieves databases and returns the results
 *
 * @return The results of the query
 */
QueryResult DatabaseConnection::selectDatabase(
        std::string database)
{

    // Execute query
    return execute(VariantList() << "USE ?" << database);
}

/**
 *
 * Retrieves tables and returns the results
 *
 * @param std::string database The name of the database to get tables from
 * @return The results of the query
 */
QueryResult DatabaseConnection::getTables(std::string database)
{

    // Execute query
    return execute(VariantList() << "SHOW TABLES FROM ?" << database);
}

/**
 *
 * Executes a query and returns the result
 *
 * @param VariantList arguments The query arguments. The first argument should
 * be the query and any subsequent arguments are the bind parameters
 *
 * @return The results from the query
 */
QueryResult DatabaseConnection::execute(VariantList arguments)
{
    int i;
    int count;
    QueryResult result;
    ResultSet *sqlResult;
    ResultSetMetaData *sqlMetadata;

    result = connect();
    if (result.error.isError) {

        // There was an error connecting. Return the result.
        return result;
    }

    PreparedStatement *sqlStatement = nullptr;

    try {

        // Prepare query
        sqlStatement = connection->prepareStatement(
            arguments.front().toString());

        if (arguments.size() > 1) {

            // Bind arguments
            int i = 1;
            for (auto it = arguments.begin() + 1; it != arguments.end(); ++it) {

                sqlStatement->setString(i, it->toString());

                i++;
            }
        }

        // Execute query
        sqlStatement->execute();

        // Fetch results
        sqlResult = sqlStatement->getResultSet();
    } catch (SQLException &e) {

        if (sqlStatement != nullptr) {

            // Free memory
            delete sqlStatement;
        }

        result.error.isError = true;
        result.error.code = e.getErrorCode();
        result.error.string = e.getSQLState() + ": " + e.what();

        return result;
    }

    sqlMetadata = sqlResult->getMetaData();

    count = sqlMetadata->getColumnCount();

    for (i = 1; i <= count; i++) {

        // Add to collection
        result.columns.push_back(sqlMetadata->getColumnName(i).asStdString());
    }

    // Read rows
    while (sqlResult->next()) {
        VariantList row;
        for (i = 1; i <= count; i++) {
            Variant column;

            switch (sqlMetadata->getColumnType(i)) {
            default:
            case DataType::UNKNOWN:
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::LONGVARCHAR:
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
            case DataType::TIMESTAMP:
            case DataType::DATE:
            case DataType::GEOMETRY:
            case DataType::ENUM:
            case DataType::SET:
            case DataType::BIGINT:
            case DataType::REAL:
            case DataType::DOUBLE:
            case DataType::DECIMAL:
                // @TODO: store binary, timestamp, date, & geometry differently
                // Also numeric types (need to be added to Variant class)
                column = sqlResult->getString(i).asStdString();
                break;
            case DataType::SQLNULL:
                column = Variant();
                break;
            case DataType::BIT:
            case DataType::TINYINT:
            case DataType::SMALLINT:
            case DataType::MEDIUMINT:
            case DataType::INTEGER:
            case DataType::NUMERIC:
                column = sqlResult->getInt(i);
                break;
            case DataType::YEAR:
                column = static_cast<unsigned short>(sqlResult->getUInt(i));
                break;
            }

            // Add column to collection
            row.push_back(column);
        }

        // Add row to collection
        result.rows.push_back(row);
    }

    // Free memory
    delete sqlResult;
    delete sqlStatement;

    return result;
}

/**
 *
 * Disconnects from the database
 *
 * @return void
 */
void DatabaseConnection::disconnect()
{
    if (connection != nullptr) {

        // Close connection
        connection->close();

        // Free memory
        delete connection;

        connection = nullptr;
    }

    if (driver != nullptr) {

        // End the thread in the MySQL driver's internals
        driver->threadEnd();

        driver = nullptr;
    }
}

/**
 *
 * Returns fields for MySQL connections
 *
 * @return void
 */
std::vector<SettingsField> DatabaseConnection::getSettingsFields()
{
    return std::vector<SettingsField>();
}

DatabaseConnection::~DatabaseConnection()
{

    // Ensure that this thread has stopped
    stop();
}

} // namespace MySQLDriver
} // namespace RabidSQL

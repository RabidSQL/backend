#include "connectionsettings.h"
#include "databaseconnection.h"
#include "databaseconnectionfactory.h"
#include "gtest/gtest.h"

namespace RabidSQL {

// @TODO: Change all MySQL-specific tests that aren't testing specific
// functionality to use SQLite once it's implemented

// Tests a MySQL connection failure
TEST(TestDatabaseConnection, ConnectionFailure) {
    ConnectionSettings settings;
    DatabaseConnection *connection;
    QueryResult result;

    // Configure connection settings
    settings.set("type", MYSQL);
    settings.set("hostname", "fake-host.fake");
    settings.set("username", "fake-username");
    settings.set("port", 1234);
    settings.set("password", "fake-password");

    // Make connection
    connection = DatabaseConnectionFactory::makeConnection(&settings);

    // Try to connect
    result = connection->connect();

    // Free memory
    delete connection;

    ASSERT_TRUE(result.error.isError);
}

// Tests a MySQL connection success
TEST(TestDatabaseConnection, ConnectionSuccess) {
    ConnectionSettings settings;
    DatabaseConnection *connection;
    QueryResult result;

    // Configure connection settings
    settings.set("type", MYSQL);
    settings.set("hostname", "localhost");
    settings.set("username", "test");

    // Make connection
    connection = DatabaseConnectionFactory::makeConnection(&settings);

    // Try to connect
    result = connection->connect();

    // Free memory
    delete connection;

    ASSERT_FALSE(result.error.isError);
}

// Tests a MySQL data fetching
TEST(TestDatabaseConnection, GetData) {
    ConnectionSettings settings;
    DatabaseConnection *connection;
    QueryResult result;

    // Configure connection settings
    settings.set("type", MYSQL);
    settings.set("hostname", "localhost");
    settings.set("username", "test");

    // Make connection
    connection = DatabaseConnectionFactory::makeConnection(&settings);

    // Check if test database exists
    std::vector<std::string> database;
    database.push_back("test");
    result = connection->getDatabases(database);

    // Free memory
    delete connection;

    // Test that query was successful
    ASSERT_FALSE(result.error.isError);

    ASSERT_EQ(1, result.rows.size());

    ASSERT_EQ("test", result.rows.front().front().toString());
}

} // namespace RabidSQL

#include "connectionsettings.h"
#include "databaseconnection.h"
#include "databaseconnectionfactory.h"
#include "gtest/gtest.h"

namespace RabidSQL {

// Tests implicitly shutting down database connections
TEST(TestDatabaseConnectionManager, reserveReleaseShutdown) {
    ConnectionSettings settings;
    DatabaseConnectionManager *manager;

    // Configure connection settings
    settings.set("type", ConnectionSettings::MYSQL);
    settings.set("hostname", "localhost");
    settings.set("username", "test");
    settings.set("max_connections", 5);

    // Make manager
    manager = DatabaseConnectionFactory::makeManager(&settings);

    // Reserve connection
    std::string uuid = manager->reserveDatabaseConnection(0);

    // Release connection
    manager->releaseDatabaseConnection(uuid);

    // Cleanup manager / free memory
    delete manager;
}

} // namespace RabidSQL

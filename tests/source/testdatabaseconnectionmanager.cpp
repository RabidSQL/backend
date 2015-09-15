#include "connectionsettings.h"
#include "databaseconnection.h"
#include "databaseconnectionfactory.h"
#include "gtest/gtest.h"

namespace RabidSQL {

// Tests shutting down database connections via manager
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

    // Give the thread enough time to startup before we check for it
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    ASSERT_EQ(1, Thread::numberOfActiveThreads());

    // Release connection
    manager->releaseDatabaseConnection(uuid);

    // Cleanup manager / free memory
    delete manager;

    ASSERT_EQ(0, Thread::numberOfActiveThreads());
}

// Tests implicitly shutting down database connections via manager
TEST(TestDatabaseConnectionManager, implicitShutdown) {
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

    // Give the thread enough time to startup before we check for it
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    ASSERT_EQ(1, Thread::numberOfActiveThreads());

    // Cleanup manager / free memory
    delete manager;

    ASSERT_EQ(0, Thread::numberOfActiveThreads());
}

} // namespace RabidSQL

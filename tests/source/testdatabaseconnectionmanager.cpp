#include "application.h"
#include "connectionsettings.h"
#include "databaseconnection.h"
#include "databaseconnectionfactory.h"
#include "smartobjecttester.h"
#include "gtest/gtest.h"

namespace RabidSQL {

class TestDatabaseConnectionManager : public ::testing::Test {
protected:
    void TearDown()
    {

        // Reset statically stored data
        SmartObjectTester::reset();
    }
};

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

// Tests retrieving database through the connection manager
TEST(TestDatabaseConnectionManager, getDatabase) {
    ConnectionSettings settings;
    DatabaseConnectionManager *manager;

    // Configure connection settings
    settings.set("type", ConnectionSettings::MYSQL);
    settings.set("hostname", "localhost");
    settings.set("username", "test");
    settings.set("max_connections", 5);

    // Make manager
    manager = DatabaseConnectionFactory::makeManager(&settings);

    // Initialize receiver
    auto receiver = new SmartObjectTester();

    // Reserve connection
    std::string uuid = manager->reserveDatabaseConnection(0, receiver);

    manager->call(uuid, Variant("uid"), QueryEvent::LIST_DATABASES,
        VariantList() << "test");

    // Give the thread enough time to startup before we check for it
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    EXPECT_EQ(1, Thread::numberOfActiveThreads());

    for (int i = 0; i < 10 && receiver->data.empty(); i++) {

        // Process events
        Application::processEvents();

        // Still waiting for data. We'll wait for up to 1 second
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Release database connection
    manager->releaseDatabaseConnection(uuid);

    // Cleanup manager / free memory
    delete manager;

    ASSERT_EQ(receiver->data.size(), 1);
    ASSERT_NE(receiver->data.find(DatabaseConnection::EXECUTED),
              receiver->data.end());

    // Get first result
    auto meta = receiver->data[DatabaseConnection::EXECUTED];

    // Free memory
    delete receiver;

    ASSERT_EQ(meta.size(), 3);
    ASSERT_EQ(meta[0], "uid");
    ASSERT_EQ(meta[1], QueryEvent::LIST_DATABASES);

    auto result = meta[2].toQueryResult();

    ASSERT_FALSE(result.error.isError);
    ASSERT_EQ(result.rows.size(), 1);
    ASSERT_EQ(result.rows.front().front(), "test");

    EXPECT_EQ(0, Thread::numberOfActiveThreads());
}

} // namespace RabidSQL

#include "Application.h"
#include "ConnectionSettings.h"
#include "DatabaseConnection.h"
#include "DatabaseConnectionFactory.h"
#include "DatabaseConnectionManager.h"
#include "NSEnums.h"
#include "smartobjecttester.h"
#include "mockapplication.h"
#include "mockconnectionsettings.h"
#include "mockdatabaseconnection.h"
#include "mocksmartobject.h"
#include "gtest/gtest.h"

using ::testing::Exactly;
using ::testing::_;
using ::testing::Return;

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
    MockApplication app;
    EXPECT_CALL(app, registerObject(_)).Times(Exactly(4));

    MockConnectionSettings settings;
    EXPECT_CALL(settings, get("type", true)).Times(Exactly(1)).WillOnce(Return(INHERIT));
    EXPECT_CALL(settings, get("max_connections", true)).Times(Exactly(1)).WillOnce(Return(5));
    EXPECT_CALL(app, unregisterObject(&settings)).Times(Exactly(1));
    // Connection MUST be a pointer because the manager is going to delete it
    // when it is finished.

    MockDatabaseConnection *parentConnection = new MockDatabaseConnection();
    MockDatabaseConnection *connection = new MockDatabaseConnection();
    EXPECT_CALL(app, unregisterObject(parentConnection)).Times(Exactly(1));
    EXPECT_CALL(app, unregisterObject(connection)).Times(Exactly(1));
    EXPECT_CALL(*connection, start()).Times(Exactly(1));

    DatabaseConnectionManager manager(parentConnection, &settings);
    EXPECT_CALL(*parentConnection, clone(&manager)).Times(Exactly(1)).WillOnce(Return(connection));
    EXPECT_CALL(*connection, join()).Times(Exactly(1));
    EXPECT_CALL(app, unregisterObject(&manager)).Times(Exactly(1));

    // Reserve connection.
    std::string uuid = manager.reserveDatabaseConnection(0);

    // Release connection
    manager.releaseDatabaseConnection(uuid);
}

// Tests implicitly shutting down database connections via manager
TEST(TestDatabaseConnectionManager, implicitShutdown) {
    MockApplication app;
    EXPECT_CALL(app, registerObject(_)).Times(Exactly(4));

    MockConnectionSettings settings;
    EXPECT_CALL(settings, get("type", true)).Times(Exactly(1)).WillOnce(Return(INHERIT));
    EXPECT_CALL(settings, get("max_connections", true)).Times(Exactly(1)).WillOnce(Return(5));
    EXPECT_CALL(app, unregisterObject(&settings)).Times(Exactly(1));
    // Connection MUST be a pointer because the manager is going to delete it
    // when it is finished.

    MockDatabaseConnection *parentConnection = new MockDatabaseConnection();
    MockDatabaseConnection *connection = new MockDatabaseConnection();
    EXPECT_CALL(app, unregisterObject(parentConnection)).Times(Exactly(1));
    EXPECT_CALL(app, unregisterObject(connection)).Times(Exactly(1));
    EXPECT_CALL(*connection, start()).Times(Exactly(1));

    DatabaseConnectionManager manager(parentConnection, &settings);
    EXPECT_CALL(*parentConnection, clone(&manager)).Times(Exactly(1)).WillOnce(Return(connection));
    EXPECT_CALL(*connection, join()).Times(Exactly(1));
    EXPECT_CALL(app, unregisterObject(&manager)).Times(Exactly(1));

    // Reserve connection.
    std::string uuid = manager.reserveDatabaseConnection(0);
}

// Tests retrieving database through the connection manager
TEST(TestDatabaseConnectionManager, getDatabase) {
    MockApplication app;
    EXPECT_CALL(app, registerObject(_)).Times(Exactly(5));

    MockConnectionSettings settings;
    EXPECT_CALL(settings, get("type", true)).Times(Exactly(1)).WillOnce(Return(INHERIT));
    EXPECT_CALL(settings, get("max_connections", true)).Times(Exactly(1)).WillOnce(Return(5));
    EXPECT_CALL(app, unregisterObject(&settings)).Times(Exactly(1));

    MockDatabaseConnection *parentConnection = new MockDatabaseConnection();
    MockDatabaseConnection *connection = new MockDatabaseConnection();
    EXPECT_CALL(app, unregisterObject(parentConnection)).Times(Exactly(1));
    EXPECT_CALL(app, unregisterObject(connection)).Times(Exactly(1));
    EXPECT_CALL(*connection, start()).Times(Exactly(1));

    DatabaseConnectionManager manager(parentConnection, &settings);
    EXPECT_CALL(*parentConnection, clone(&manager)).Times(Exactly(1)).WillOnce(Return(connection));
    EXPECT_CALL(*connection, join()).Times(Exactly(1));
    EXPECT_CALL(app, unregisterObject(&manager)).Times(Exactly(1));

    // Initialize receiver
    MockSmartObject receiver;
    EXPECT_CALL(app, unregisterObject(&receiver)).Times(Exactly(1));

    // Reserve connection
    std::string uuid = manager.reserveDatabaseConnection(0, &receiver);

    EXPECT_CALL(*connection, call(Variant("uid"), LIST_DATABASES, VariantVector() << "test")).Times(Exactly(1));
    manager.call(uuid, Variant("uid"), LIST_DATABASES,
        VariantVector() << "test");

    // Release database connection
    manager.releaseDatabaseConnection(uuid);
}

// Tests retrieving all databases through the connection manager. This is really
// to test optional query arguments. This is called here instead of in
// TestDatabaseConnection because after calling call() on a connection, nothing
// will actually happen until the thread picks it up, which will be never if
// there is no manager.
TEST(TestDatabaseConnectionManager, getAllDatabases) {
    MockApplication app;
    EXPECT_CALL(app, registerObject(_)).Times(Exactly(5));

    MockConnectionSettings settings;
    EXPECT_CALL(settings, get("type", true)).Times(Exactly(1)).WillOnce(Return(INHERIT));
    EXPECT_CALL(settings, get("max_connections", true)).Times(Exactly(1)).WillOnce(Return(5));
    EXPECT_CALL(app, unregisterObject(&settings)).Times(Exactly(1));

    MockDatabaseConnection *parentConnection = new MockDatabaseConnection();
    MockDatabaseConnection *connection = new MockDatabaseConnection();
    EXPECT_CALL(app, unregisterObject(parentConnection)).Times(Exactly(1));
    EXPECT_CALL(app, unregisterObject(connection)).Times(Exactly(1));
    EXPECT_CALL(*connection, start()).Times(Exactly(1));

    DatabaseConnectionManager manager(parentConnection, &settings);
    EXPECT_CALL(*parentConnection, clone(&manager)).Times(Exactly(1)).WillOnce(Return(connection));
    EXPECT_CALL(*connection, join()).Times(Exactly(1));
    EXPECT_CALL(app, unregisterObject(&manager)).Times(Exactly(1));

    // Initialize receiver
    MockSmartObject receiver;
    EXPECT_CALL(app, unregisterObject(&receiver)).Times(Exactly(1));

    // Reserve connection
    std::string uuid = manager.reserveDatabaseConnection(0, &receiver);

    EXPECT_CALL(*connection, call(Variant("uid"), LIST_DATABASES, VariantVector())).Times(Exactly(1));
    manager.call(uuid, Variant("uid"), LIST_DATABASES, VariantVector());

    // Release database connection
    manager.releaseDatabaseConnection(uuid);
}

} // namespace RabidSQL

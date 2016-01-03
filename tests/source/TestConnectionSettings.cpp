#include "ConnectionSettings.h"
#include "gtest/gtest.h"

namespace RabidSQL {

class TestConnectionSettings : public ::testing::Test {
protected:
    void SetUp()
    {
        filename = "/tmp/rabidsql-test-connection-settings";
    }

    void TearDown()
    {
        // Unlink temp file
        std::remove(filename.c_str());
    }

    std::string filename;
};

// Tests setters and getters
TEST_F(TestConnectionSettings, SetterGetter) {
    ConnectionSettings settings;

    // Configure connection settings
    settings.set("type", MYSQL);
    settings.set("hostname", "test");
    settings.set("port", 1234);

    ASSERT_EQ(MYSQL, settings.get("type").toUInt());
    ASSERT_EQ("test", settings.get("hostname").toString());
    ASSERT_EQ(1234, settings.get("port").toUInt());
}

TEST_F(TestConnectionSettings, Clone) {
    ConnectionSettings *settings = new ConnectionSettings();

    // Configure connection settings
    settings->set("type", MYSQL);
    settings->set("hostname", "test");
    settings->set("port", 1234);

    ConnectionSettings *clone = settings->clone();
    delete settings;

    ASSERT_EQ(MYSQL, clone->get("type").toUInt());
    ASSERT_EQ("test", clone->get("hostname").toString());
    ASSERT_EQ(1234, clone->get("port").toUInt());

    delete clone;
}

// Tests reading and writing binary files (single connection)
TEST_F(TestConnectionSettings, BinaryIOSingleConnection) {
    std::vector<ConnectionSettings *> connections;
    ConnectionSettings *connection = new ConnectionSettings();

    // Configure connection settings
    connection->set("type", MYSQL);
    connection->set("hostname", "test");
    connection->set("port", 1234);

    // Add to collection
    connections.push_back(connection);

    // Save to file
    ConnectionSettings::save(connections, BINARY, filename);

    // Free memory
    delete connection;

    // Load it up
    connections = ConnectionSettings::load(BINARY, filename);

    // Make sure exactly 1 connection was returned
    ASSERT_EQ(1, connections.size());

    connection = connections.front();

    ASSERT_EQ(MYSQL, connection->get("type").toUInt());
    ASSERT_EQ("test", connection->get("hostname").toString());
    ASSERT_EQ(1234, connection->get("port").toUInt());

    // Free memory
    delete connection;
}

// Tests reading and writing binary files (multiple connections)
TEST_F(TestConnectionSettings, BinaryIOMultiConnections) {
    std::vector<ConnectionSettings *> connections;
    ConnectionSettings *connection1 = new ConnectionSettings();
    ConnectionSettings *connection2 = new ConnectionSettings();

    // Configure connection settings
    connection1->set("type", MYSQL);
    connection1->set("hostname", "test");
    connection1->set("port", 1234);

    // Configure second connection settings
    connection2->set("type", MYSQL);
    connection2->set("hostname", "test2");
    connection2->set("port", 3306);

    // Add to collection
    connections.push_back(connection1);
    connections.push_back(connection2);

    // Save to file
    ConnectionSettings::save(connections, FileFormat::BINARY, filename);

    // Free memory
    delete connection1;
    delete connection2;

    // Load it up
    connections = ConnectionSettings::load(FileFormat::BINARY, filename);

    // Make sure exactly 1 connection was returned
    ASSERT_EQ(2, connections.size());

    connection1 = connections[0];
    connection2 = connections[1];

    ASSERT_EQ(MYSQL, connection1->get("type").toUInt());
    ASSERT_EQ("test", connection1->get("hostname").toString());
    ASSERT_EQ(1234, connection1->get("port").toUInt());

    ASSERT_EQ(MYSQL, connection2->get("type").toUInt());
    ASSERT_EQ("test2", connection2->get("hostname").toString());
    ASSERT_EQ(3306, connection2->get("port").toUInt());

    // Free memory
    delete connection1;
    delete connection2;
}

// Tests reading and writing binary files (multiple connection with inheritance)
TEST_F(TestConnectionSettings, BinaryIOMultiConnectionInheritance) {
    std::vector<ConnectionSettings *> connections;
    ConnectionSettings *connection1 = new ConnectionSettings();
    ConnectionSettings *connection2 = new ConnectionSettings();

    // Configure connection settings
    connection1->set("type", MYSQL);
    connection1->set("hostname", "test");
    connection1->set("port", 1234);

    // Configure second connection settings
    connection2->set("type", MYSQL);
    connection2->set("hostname", "test2");
    connection2->set("port", 3306);

    // Set parent
    connection2->setParent(connection1);

    // Add to collection
    connections.push_back(connection1);
    connections.push_back(connection2);

    // Save to file
    ConnectionSettings::save(connections, BINARY, filename);

    // Free memory
    delete connection1;

    // Load it up
    connections = ConnectionSettings::load(BINARY, filename);

    // Make sure exactly 1 connection was returned
    ASSERT_EQ(2, connections.size());

    connection1 = connections[0];
    connection2 = connections[1];

    ASSERT_EQ(connection1, connection2->getParent());

    ASSERT_EQ(MYSQL, connection1->get("type").toUInt());
    ASSERT_EQ("test", connection1->get("hostname").toString());
    ASSERT_EQ(1234, connection1->get("port").toUInt());

    ASSERT_EQ(MYSQL, connection2->get("type").toUInt());
    ASSERT_EQ("test2", connection2->get("hostname").toString());
    ASSERT_EQ(3306, connection2->get("port").toUInt());

    // Free memory
    delete connection1;
}

// Tests reading and writing JSON files (single connection)
TEST_F(TestConnectionSettings, JsonIOSingleConnection) {
    std::vector<ConnectionSettings *> connections;
    ConnectionSettings *connection = new ConnectionSettings();

    // Configure connection settings
    connection->set("type", MYSQL);
    connection->set("hostname", "test");
    connection->set("port", 1234);

    // Add to collection
    connections.push_back(connection);

    // Save to file
    ConnectionSettings::save(connections, JSON, filename);

    // Free memory
    delete connection;

    // Load it up
    connections = ConnectionSettings::load(JSON, filename);

    // Make sure exactly 1 connection was returned
    ASSERT_EQ(1, connections.size());

    connection = connections.front();

    ASSERT_EQ(MYSQL, connection->get("type").toUInt());
    ASSERT_EQ("test", connection->get("hostname").toString());
    ASSERT_EQ(1234, connection->get("port").toUInt());

    // Free memory
    delete connection;
}

// Tests reading and writing JSON files (multiple connections)
TEST_F(TestConnectionSettings, JsonIOMultiConnection) {
    std::vector<ConnectionSettings *> connections;
    ConnectionSettings *connection1 = new ConnectionSettings();
    ConnectionSettings *connection2 = new ConnectionSettings();

    // Configure connection settings
    connection1->set("type", MYSQL);
    connection1->set("hostname", "test");
    connection1->set("port", 1234);

    // Configure second connection settings
    connection2->set("type", MYSQL);
    connection2->set("hostname", "test2");
    connection2->set("port", 3306);

    // Add to collection
    connections.push_back(connection1);
    connections.push_back(connection2);

    // Save to file
    ConnectionSettings::save(connections, JSON, filename);

    // Free memory
    delete connection1;
    delete connection2;

    // Load it up
    connections = ConnectionSettings::load(JSON, filename);

    // Make sure exactly 2 connections were returned
    ASSERT_EQ(2, connections.size());

    connection1 = connections[0];
    connection2 = connections[1];

    ASSERT_EQ(MYSQL, connection1->get("type").toUInt());
    ASSERT_EQ("test", connection1->get("hostname").toString());
    ASSERT_EQ(1234, connection1->get("port").toUInt());

    ASSERT_EQ(MYSQL, connection2->get("type").toUInt());
    ASSERT_EQ("test2", connection2->get("hostname").toString());
    ASSERT_EQ(3306, connection2->get("port").toUInt());

    // Free memory
    delete connection1;
    delete connection2;
}

// Tests reading and writing JSON files (multiple connection with inheritance)
TEST_F(TestConnectionSettings, JsonIOMultiConnectionInheritance) {
    std::vector<ConnectionSettings *> connections;
    ConnectionSettings *connection1 = new ConnectionSettings();
    ConnectionSettings *connection2 = new ConnectionSettings();

    // Configure connection settings
    connection1->set("type", MYSQL);
    connection1->set("hostname", "test");
    connection1->set("port", 1234);

    // Configure second connection settings
    connection2->set("type", MYSQL);
    connection2->set("hostname", "test2");
    connection2->set("port", 3306);

    // Set parent
    connection2->setParent(connection1);

    // Add to collection
    connections.push_back(connection1);
    connections.push_back(connection2);

    // Save to file
    ConnectionSettings::save(connections, JSON, filename);

    // Free memory
    delete connection1;

    // Load it up
    connections = ConnectionSettings::load(JSON, filename);

    // Make sure exactly 1 connection was returned
    ASSERT_EQ(2, connections.size());

    connection1 = connections[0];
    connection2 = connections[1];

    ASSERT_EQ(connection1, connection2->getParent());

    ASSERT_EQ(MYSQL, connection1->get("type").toUInt());
    ASSERT_EQ("test", connection1->get("hostname").toString());
    ASSERT_EQ(1234, connection1->get("port").toUInt());

    ASSERT_EQ(MYSQL, connection2->get("type").toUInt());
    ASSERT_EQ("test2", connection2->get("hostname").toString());
    ASSERT_EQ(3306, connection2->get("port").toUInt());

    // Free memory
    delete connection1;
}

} // namespace RabidSQL

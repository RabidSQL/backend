#ifndef RABIDSQL_MOCKDATABASECONNECTION_H
#define RABIDSQL_MOCKDATABASECONNECTION_H

#include "drivers/DatabaseConnection.h"

#include "gmock/gmock.h"

namespace RabidSQL {

class ConnectionSettings;
class DatabaseConnectionManager;
class MockDatabaseConnection : public DatabaseConnection {
public:
    MockDatabaseConnection() : DatabaseConnection(nullptr) {}

    MOCK_METHOD0(disconnect, void());
    MOCK_METHOD0(connect, QueryResult());
    MOCK_METHOD1(execute, QueryResult(VariantVector));
    MOCK_METHOD1(getDatabases, QueryResult(std::vector<std::string>));
    MOCK_METHOD1(getTables, QueryResult(std::string));
    MOCK_METHOD1(selectDatabase, QueryResult(std::string));
    MOCK_METHOD1(killQuery, QueryResult(std::string));
    MOCK_METHOD1(clone, DatabaseConnection *(DatabaseConnectionManager *));
    MOCK_METHOD3(call, void(Variant, QueryEvent, VariantVector));
    MOCK_METHOD0(run, void());
    MOCK_METHOD0(join, void());
    MOCK_METHOD0(start, void());
    MOCK_METHOD1(stop, void(bool));
};

} // namespace RabidSQL

#endif // RABIDSQL_MOCKDATABASECONNECTION_H

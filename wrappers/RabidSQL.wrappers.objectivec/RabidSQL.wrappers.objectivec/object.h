#ifndef OBJECT_H
#define OBJECT_H

#include "connectionsettings.h"
#include "databaseconnection.h"
#include "databaseconnectionmanager.h"
#include "structures.h"
#include "variant.h"

namespace RabidSQL {
    class ConnectionSettings;
    class DatabaseConnection;
    class DatabaseConnectionManager;
    class Message;
    class Variant;
    struct QueryResult;
    struct QueryError;
}

class Object {

public:
    static QueryResult* convert(const RabidSQL::QueryResult &value);

    static QueryError* convert(const RabidSQL::QueryError &value);

    static Message* convert(const RabidSQL::Message &value);

    static Variant *wrap(const RabidSQL::Variant &value);
    static RabidSQL::Variant unwrap(const Variant *value);

    static ConnectionSettings *wrap(RabidSQL::ConnectionSettings *value);
    static RabidSQL::ConnectionSettings *unwrap(ConnectionSettings *value);

    static DatabaseConnection *wrap(RabidSQL::DatabaseConnection *value);
    static RabidSQL::DatabaseConnection *unwrap(DatabaseConnection *value);

    static DatabaseConnectionManager *wrap(
        RabidSQL::DatabaseConnectionManager *value);
    static RabidSQL::DatabaseConnectionManager *unwrap(
        DatabaseConnectionManager *value);
};

#endif /* OBJECT_H */

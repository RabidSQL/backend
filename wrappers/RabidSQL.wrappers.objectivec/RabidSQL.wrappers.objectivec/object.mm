#import <Foundation/Foundation.h>

#include "object.h"
#include "prototypes.h"
#include "structures.h"

#include "logic/include/connectionsettings.h"
#include "logic/include/databaseconnection.h"
#include "logic/include/databaseconnectionmanager.h"
#include "logic/include/variant.h"
#include "logic/include/structs.h"

QueryError* Object::convert(const RabidSQL::QueryError &value)
{
    QueryError *error = [[QueryError alloc] init];

    error.isError = value.isError;
    error.code = wrap(value.code);
    error.string = [NSString stringWithUTF8String:value.string.c_str()];

    return error;
}

QueryResult* Object::convert(const RabidSQL::QueryResult &value)
{
    QueryResult *result = [[QueryResult alloc] init];

    result.uid = wrap(value.uid);
    result.is_valid = value.is_valid;
    result.affected_rows = value.affected_rows;
    result.num_rows = value.num_rows;
    result.event = static_cast<QueryEvent>(value.event);
    result.error = convert(value.error);

    for (auto it = value.columns.begin(); it != value.columns.end(); ++it) {

        [result.columns addObject:[NSString stringWithUTF8String:(it->c_str())]];
    }

    for (auto it = value.rows.begin(); it != value.rows.end(); ++it) {

        NSMutableArray *row = [[NSMutableArray alloc] init];

        for (auto rit = (*it).begin(); rit != (*it).end(); ++rit) {

            [row addObject: wrap(*rit)];
        }

        [result.rows addObject: row];
    }

    return result;
}

Variant *Object::wrap(const RabidSQL::Variant &value)
{
    Variant *variant = [[Variant alloc] init];
    variant.data->object = value;

    return variant;
}

RabidSQL::Variant Object::unwrap(const Variant *value)
{
    return value.data->object;
}

Message* Object::convert(const RabidSQL::Message &value)
{
    Message *message = [[Message alloc] init];
    message.type = static_cast<MessageType>(value.type);
    message.label = [NSString stringWithUTF8String:value.label.c_str()];
    message.data = wrap(value.data);
    return message;
}

ConnectionSettings *Object::wrap(RabidSQL::ConnectionSettings *value)
{
    ConnectionSettings *connectionSettings = [[ConnectionSettings alloc] init];

    connectionSettings.data->object = value;

    return connectionSettings;
}

RabidSQL::ConnectionSettings *Object::unwrap(ConnectionSettings *value)
{
    return value.data->object;
}

DatabaseConnection *Object::wrap(RabidSQL::DatabaseConnection *value)
{
    DatabaseConnection *databaseConnection = [[DatabaseConnection alloc] init];

    databaseConnection.ddata->object = value;

    return databaseConnection;
}

RabidSQL::DatabaseConnection *Object::unwrap(DatabaseConnection *value)
{
    return value.ddata->object;
}

DatabaseConnectionManager *Object::wrap(
        RabidSQL::DatabaseConnectionManager *value)
{
    DatabaseConnectionManager *manager = [[DatabaseConnectionManager alloc] init];

    manager.mdata->object = value;

    return manager;
}

RabidSQL::DatabaseConnectionManager *Object::unwrap(
        DatabaseConnectionManager *value)
{
    return value.mdata->object;
}

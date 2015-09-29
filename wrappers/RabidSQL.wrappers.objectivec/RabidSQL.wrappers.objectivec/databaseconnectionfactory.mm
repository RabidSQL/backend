#import <Foundation/Foundation.h>

#include "connectionsettings.h"
#include "databaseconnectionfactory.h"
#include "object.h"

#include "logic/include/databaseconnectionfactory.h"

@implementation DatabaseConnectionFactory

+ (DatabaseConnection *) makeConnection: (ConnectionSettings *) settings
{
    return Object::wrap(RabidSQL::DatabaseConnectionFactory::makeConnection(Object::unwrap(settings)));
}

+ (DatabaseConnectionManager *) makeManager: (ConnectionSettings *) settings
{
    auto manager = RabidSQL::DatabaseConnectionFactory::makeManager(Object::unwrap(settings));
    return Object::wrap(manager);
//    return Object::wrap(RabidSQL::DatabaseConnectionFactory::makeManager(Object::unwrap(settings)));
}

@end

#ifndef DATABASECONNECTIONFACTORY_H
#define DATABASECONNECTIONFACTORY_H

#include <Foundation/Foundation.h>

@class DatabaseConnectionManager;
@class ConnectionSettings;
@class DatabaseConnection;
@interface DatabaseConnectionFactory : NSObject

+ (DatabaseConnection *) makeConnection: (ConnectionSettings *) settings;
+ (DatabaseConnectionManager *) makeManager: (ConnectionSettings *) settings;

@end

#endif /* DATABASECONNECTIONFACTORY_H */

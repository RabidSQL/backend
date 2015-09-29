#ifndef DATABASECONNECTIONMANAGER_H
#define DATABASECONNECTIONMANAGER_H

#include <Foundation/Foundation.h>

#include "logic/include/enums.h"

#include "smartobject.h"

@class ConnectionSettings;
@class DatabaseConnection;
@class Variant;

struct RDatabaseConnectionManager;

@interface DatabaseConnectionManager : SmartObject

- (id) init;
- (id) init: (DatabaseConnection *) mainConnection
withSettings: (ConnectionSettings *) settings;
- (NSString *) reserveDatabaseConnection: (int) expiry
                            withReceiver: (SmartObject *) receiver;
- (void) releaseDatabaseConnection: (NSString *) uuid;
- (void) call: (NSString *) uuid
      withUid: (Variant *) uid
      ofEvent: (QueryEvent) event
withArguments: (NSMutableArray *) arguments;
- (void) blockingCall: (NSString *) uuid
              withUid: (Variant *) uid
              ofEvent: (QueryEvent) event
        withArguments: (NSMutableArray *) arguments;
- (void) killQuery: (NSString *) uuid;
- (ConnectionType) getType;
- (void) dealloc;

@property struct RDatabaseConnectionManager *mdata;

@end

#endif /* DATABASECONNECTIONMANAGER_H */

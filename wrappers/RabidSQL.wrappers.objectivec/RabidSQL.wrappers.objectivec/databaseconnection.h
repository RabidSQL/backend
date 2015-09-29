#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <Foundation/Foundation.h>

#include "smartobject.h"

struct RDatabaseConnection;

@interface DatabaseConnection: SmartObject

- (id) init;
- (void) dealloc;
+ (int) getExecutedId;

@property struct RDatabaseConnection *ddata;

@end
#endif /* DATABASECONNECTION_H */

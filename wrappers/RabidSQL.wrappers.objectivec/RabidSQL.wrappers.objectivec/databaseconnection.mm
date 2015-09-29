#import <Foundation/Foundation.h>

#include "databaseconnection.h"
#include "prototypes.h"

#include "logic/include/databaseconnection.h"

@implementation DatabaseConnection

- (id) init
{
    self = [super init];

    if (self) {

        _ddata = new RDatabaseConnection();
        _ddata->object = nullptr;
    }

    return self;
}

- (void) dealloc
{
    if (_ddata->object != nullptr) {

        delete _ddata->object;
    }

    delete _ddata;
}

+ (int) getExecutedId
{
    return RabidSQL::DatabaseConnection::EXECUTED;
}

@end
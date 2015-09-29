#import <Foundation/Foundation.h>

#include "logic/include/databaseconnectionmanager.h"
#include "logic/include/smartobject.h"

#include "databaseconnectionmanager.h"
#include "object.h"
#include "prototypes.h"
#include "smartobjectwrapper.h"
#include "smartobject.h"

@implementation DatabaseConnectionManager

- (id) init
{
    self = [super init];

    if (self) {

        _mdata = new RDatabaseConnectionManager();
        _mdata->object = nullptr;
    }

    return self;
}

- (id) init: (DatabaseConnection *) mainConnection withSettings: (ConnectionSettings *) settings
{
    self = [self init];

    if (self) {

        _mdata->object = new RabidSQL::DatabaseConnectionManager(
            Object::unwrap(mainConnection), Object::unwrap(settings));
    }

    return self;
}

- (NSString *) reserveDatabaseConnection: (int) expiry withReceiver: (SmartObject *) receiver
{
    return [NSString stringWithUTF8String:_mdata->object->reserveDatabaseConnection(expiry, receiver.data->object).c_str()];
}

- (void) releaseDatabaseConnection: (NSString *) uuid
{
    _mdata->object->releaseDatabaseConnection([uuid UTF8String]);
}

- (void) call: (NSString *) uuid
      withUid: (Variant *) uid
      ofEvent: (QueryEvent) event
withArguments: (NSMutableArray *) arguments
{
    Variant *variant = [[Variant alloc] initWithVariantList: arguments];
    _mdata->object->call(std::string([uuid UTF8String]), Object::unwrap(uid),
                         static_cast<RabidSQL::QueryEvent>(event),
                         Object::unwrap(variant).toVariantVector());
}

- (void) blockingCall: (NSString *) uuid
              withUid: (Variant *) uid
              ofEvent: (QueryEvent) event
        withArguments: (NSMutableArray *) arguments
{
    Variant *variant = [[Variant alloc] initWithVariantList: arguments];
    _mdata->object->call(std::string([uuid UTF8String]), Object::unwrap(uid),
                         static_cast<RabidSQL::QueryEvent>(event),
                         Object::unwrap(variant).toVariantVector(), true);
}

- (void) killQuery: (NSString *) uuid
{
    _mdata->object->killQuery([uuid UTF8String]);
}

- (ConnectionType) getType
{
    return static_cast<ConnectionType>(_mdata->object->getType());
}

- (void) dealloc
{
    delete _mdata->object;
    delete _mdata;
}

@end
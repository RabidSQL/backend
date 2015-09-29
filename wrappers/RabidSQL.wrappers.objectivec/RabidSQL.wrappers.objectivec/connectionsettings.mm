#import <Foundation/Foundation.h>

#include "connectionsettings.h"
#include "object.h"
#include "prototypes.h"

#include "logic/include/connectionsettings.h"

@implementation ConnectionSettings

- (id) init
{
    self = [super init];

    if (self) {

        _data = new RConnectionSettings();
        _data->object = new RabidSQL::ConnectionSettings();
    }

    return self;
}

- (void) remove: (NSString *) key
{
    _data->object->remove([key UTF8String]);
}

- (bool) contains: (NSString *) key
{
    return _data->object->contains([key UTF8String]);
}

- (Variant *) get: (NSString *) key
{
    return Object::wrap(_data->object->get([key UTF8String], true));
}

- (Variant *) get: (NSString *) key bubbleUp: (bool) bubble
{
    return Object::wrap(_data->object->get([key UTF8String], bubble));
}

- (void) set: (NSString *) key withValue:(Variant *) value
{
    _data->object->set([key UTF8String], Object::unwrap(value));
}

- (ConnectionType) getType
{
    return static_cast<ConnectionType>(_data->object->getType());
}

- (void) dealloc
{
    delete _data->object;
    delete _data;
}

+ (NSMutableArray *) load: (FileFormat) format filename: (NSString *) filename
{
    NSMutableArray *array = [[NSMutableArray alloc] init];

    auto settings = RabidSQL::ConnectionSettings::load(
        static_cast<RabidSQL::FileFormat>(format), [filename UTF8String]);

    for (auto it = settings.begin(); it != settings.end(); ++it) {

        [array addObject:(Object::wrap(*it))];
    }

    return array;
}

+ (void) save: (NSMutableArray *) settings
     inFormat: (FileFormat) format
 withFilename: (NSString *) filename
{
    std::vector<RabidSQL::ConnectionSettings *> connections;

    for (ConnectionSettings *connection in settings) {

        connections.push_back(Object::unwrap(connection));
    }

    RabidSQL::ConnectionSettings::save(connections,
                                       static_cast<RabidSQL::FileFormat>(format),
                                       [filename UTF8String]);
}

@end

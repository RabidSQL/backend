#import <Foundation/Foundation.h>

#include "object.h"
#include "prototypes.h"
#include "variant.h"
#include "logic/include/structs.h"
#include "logic/include/variant.h"

@implementation Variant : NSObject

- (id) init
{
    self = [super init];

    if (self) {

        _data = new RVariant();
    }

    return self;
}

- (id) initWithNumber: (NSNumber *) value
             withType: (DataType) type
{
    self = [self init];

    if (self) {

        [self setNumber: value withType: type];
    }

    return self;
}

- (id) initWithString: (NSString *) value
{
    self = [self init];

    if (self) {

        [self setString: value];
    }

    return self;
}

- (id) initWithStringList: (NSMutableArray *) value
{
    self = [self init];

    if (self) {

        [self setStringList: value];
    }

    return self;
}

- (id) initWithVariantList: (NSMutableArray *) value
{
    self = [self init];

    if (self) {

        [self setVariantList: value];
    }

    return self;
}

- (id) initWithVariantDictionary: (NSMutableDictionary *) value
{
    self = [self init];

    if (self) {

        [self setVariantDictionary: value];
    }

    return self;
}

- (void) setNumber: (NSNumber *) value
             withType: (DataType) type
{
    _data = new RVariant();
    switch (type) {
        case D_ULONG:
            _data->object = [value unsignedLongValue];
            break;
        case D_UINT:
            _data->object = [value unsignedIntegerValue];
            break;
        case D_INT:
            _data->object = [value integerValue];
            break;
        case D_USHORT:
            _data->object = [value unsignedShortValue];
            break;
        case D_SHORT:
            _data->object = [value shortValue];
            break;
        case D_FLOAT:
            _data->object = [value floatValue];
            break;
        case D_DOUBLE:
            _data->object = [value doubleValue];
            break;
        case D_BOOLEAN:
            _data->object = [value boolValue];
            break;
        case D_LONG:
        default:
            _data->object = [value longValue];
            break;
    }
}

- (void) setString: (NSString *) value
{
    _data = new RVariant();
    _data->object = [value UTF8String];
}

- (void) setStringList: (NSMutableArray *) value
{
    std::vector<std::string> vector;

    for (NSString *item in value) {

        // Add to collection
        vector.push_back([item UTF8String]);
    }

    _data->object = vector;
}

- (void) setVariantList: (NSMutableArray *) value
{
    RabidSQL::VariantVector vector;

    for (Variant *item in value) {

        // Add to collection
        vector.push_back(item->_data->object);
    }

    _data->object = vector;
}

- (void) setVariantDictionary: (NSMutableDictionary *) value
{
    RabidSQL::VariantMap map;

    for (NSString *key in value) {

        Variant *val = [value objectForKey:key];

        map[[key UTF8String]] = val->_data->object;
    }

    _data->object = map;
}

- (DataType) getType
{
    return static_cast<DataType>(_data->object.getType());
}

- (NSNumber *) toNumber
{
    switch (static_cast<DataType>([self getType])) {
        case D_ULONG:
            return [NSNumber numberWithUnsignedLong:_data->object.toULong()];
        case D_UINT:
            return [NSNumber numberWithUnsignedInt:_data->object.toUInt()];
        case D_INT:
            return [NSNumber numberWithInteger:_data->object.toInt()];
        case D_USHORT:
            return [NSNumber numberWithUnsignedShort:_data->object.toUShort()];
        case D_SHORT:
            return [NSNumber numberWithShort:_data->object.toShort()];
        case D_FLOAT:
            return [NSNumber numberWithFloat:_data->object.toFloat()];
        case D_DOUBLE:
            return [NSNumber numberWithDouble:_data->object.toDouble()];
        case D_BOOLEAN:
            return [NSNumber numberWithBool:_data->object.toBool()];
        case D_LONG:
        default:
            return [NSNumber numberWithLong:_data->object.toLong()];
    }
}

- (NSString *) toString
{
    return [NSString stringWithUTF8String:_data->object.toString().c_str()];
}

- (NSMutableArray *) toStringList
{
    auto vector = _data->object.toStringVector();
    NSMutableArray *list = [[NSMutableArray alloc] init];

    for (auto it = vector.begin(); it != vector.end(); ++it) {

        [list addObject:[NSString stringWithUTF8String:(it->c_str())]];
    }

    return list;
}

- (NSMutableArray *) toVariantList
{
    auto vector = _data->object.toVariantVector();
    NSMutableArray *list = [[NSMutableArray alloc] init];

    for (auto it = vector.begin(); it != vector.end(); ++it) {

        [list addObject:(Object::wrap(*it))];
    }

    return list;
}

- (NSMutableDictionary *) toVariantDictionary
{
    auto map = _data->object.toVariantMap();
    NSMutableDictionary *dictionary = [[NSMutableDictionary alloc] init];

    for (auto it = map.begin(); it != map.end(); ++it) {

        dictionary[[NSString stringWithUTF8String:it->first.c_str()]] = Object::wrap(it->second);
    }

    return dictionary;
}

- (QueryResult *) toQueryResult
{
    return Object::convert(_data->object.toQueryResult());
}

- (bool) isEqual: (Variant *) value
{
    if (value == self) {

        return true;
    }

    return _data->object == value->_data->object;
}

- (void) dealloc
{
    delete _data;
}

@end

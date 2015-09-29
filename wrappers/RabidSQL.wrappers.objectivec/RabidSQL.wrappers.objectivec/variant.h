#ifndef VARIANT_H
#define VARIANT_H

#include <Foundation/Foundation.h>

#include "logic/include/enums.h"

#include "structures.h"

struct RVariant;

@interface Variant : NSObject

- (id) init;
- (id) initWithNumber: (NSNumber *) value
             withType: (DataType) type;
- (id) initWithString: (NSString *) value;
- (id) initWithStringList: (NSMutableArray *) value;
- (id) initWithVariantList: (NSMutableArray *) value;
- (id) initWithVariantDictionary: (NSMutableDictionary *) value;
- (void) setNumber: (NSNumber *) value
          withType: (DataType) type;
- (void) setString: (NSString *) value;
- (void) setStringList: (NSMutableArray *) value;
- (void) setVariantList: (NSMutableArray *) value;
- (void) setVariantDictionary: (NSMutableDictionary *) value;
- (DataType) getType;
- (NSNumber *) toNumber;
- (NSString *) toString;
- (NSMutableArray *) toStringList;
- (NSMutableArray *) toVariantList;
- (NSMutableDictionary *) toVariantDictionary;
- (QueryResult *) toQueryResult;
- (bool) isEqual: (Variant *) value;
- (void) dealloc;

@property struct RVariant *data;

@end

#endif /* VARIANT_H */

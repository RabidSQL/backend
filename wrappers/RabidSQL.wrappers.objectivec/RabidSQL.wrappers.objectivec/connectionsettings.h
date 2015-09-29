#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include <Foundation/Foundation.h>

#include "logic/include/enums.h"

@class Variant;

struct RConnectionSettings;

@interface ConnectionSettings : NSObject

- (id) init;
- (void) remove: (NSString *) key;
- (bool) contains: (NSString *) key;
- (Variant *) get: (NSString *) key;
- (Variant *) get: (NSString *) key
         bubbleUp: (bool) bubble;
- (void) set: (NSString *) key
   withValue: (Variant *) value;
- (ConnectionType) getType;
- (void) dealloc;

+ (NSMutableArray *) load: (FileFormat) format
                 filename:(NSString *) filename;
+ (void) save: (NSMutableArray *) settings
     inFormat: (FileFormat) format
 withFilename: (NSString *) filename;

@property struct RConnectionSettings *data;

@end

#endif /* CONNECTIONSETTINGS_H */

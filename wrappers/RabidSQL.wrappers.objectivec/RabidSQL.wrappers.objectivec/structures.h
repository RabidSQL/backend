#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <Foundation/Foundation.h>

#include "logic/include/enums.h"

@class Variant;

@interface Message: NSObject

- (id) init;
@property MessageType type;
@property NSString *label;
@property Variant *data;

@end

@interface QueryError: NSObject

- (id) init;
@property bool isError;
@property Variant *code;
@property NSString *string;

@end

@interface QueryResult: NSObject

- (id) init;
@property Variant *uid;
@property bool is_valid;
@property int affected_rows;
@property int num_rows;
@property QueryEvent event;
@property QueryError *error;
@property NSMutableArray *columns;
@property NSMutableArray *rows;

@end

#endif /* STRUCTURES_H */

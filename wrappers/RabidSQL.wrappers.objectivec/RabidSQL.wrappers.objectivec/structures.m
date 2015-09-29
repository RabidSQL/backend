#import <Foundation/Foundation.h>

#include "structures.h"
#include "variant.h"

@implementation Message

- (id) init {

    self = [super init];

    if (self) {

        _label = @"";
        _data = [[Variant alloc] init];
    }

    return self;
}

@end

@implementation QueryError

- (id) init {

    self = [super init];

    if (self) {

        _isError = false;
        _code = [[Variant alloc] init];
        _string = @"";
    }

    return self;
}

@end

@implementation QueryResult

- (id) init {

    self = [super init];

    if (self) {

        _uid = [[Variant alloc] init];
        _is_valid = false;
        _affected_rows = 0;
        _num_rows = 0;
        _event = NO_EVENT;
        _error = [[QueryError alloc] init];
        _columns = [[NSMutableArray alloc] init];
        _rows = [[NSMutableArray alloc] init];
    }

    return self;
}

@end

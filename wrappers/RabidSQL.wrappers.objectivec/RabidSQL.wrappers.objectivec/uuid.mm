#import "uuid.h"
#include "logic/include/uuid.h"

@implementation UUID

+ (NSString *) makeUUID
{
    return [NSString stringWithUTF8String:RabidSQL::UUID::makeUUID().c_str()];
}

+ (NSString *) threadSafeMakeUUID
{
    return [NSString stringWithUTF8String:RabidSQL::UUID::threadSafeMakeUUID().c_str()];
}

@end

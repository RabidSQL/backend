#ifndef UUID_H
#define UUID_H

#import <Foundation/Foundation.h>

@interface UUID : NSObject

+ (NSString *) makeUUID;
+ (NSString *) threadSafeMakeUUID;

@end

#endif /* UUID_H */

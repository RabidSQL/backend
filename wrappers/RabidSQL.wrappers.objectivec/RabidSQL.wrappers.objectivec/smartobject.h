#ifndef SMARTOBJECT_H
#define SMARTOBJECT_H

#include <Foundation/Foundation.h>

struct RSmartObject;

@interface SmartObject : NSObject

- (id) init;
- (void) dealloc;
- (void) queueData: (int) id withArguments: (NSMutableArray *) arguments;
- (void) processQueueItem: (int) id withArguments: (NSMutableArray *) arguments;
- (void) connectQueue: (int) i
         withReceiver: (SmartObject *) receiver;
- (void) disconnectQueue;
- (void) disconnectQueueById: (int) id;
- (void) disconnectQueueByReceiver: (SmartObject *) receiver;
- (void) disconnectQueueByReceiver: (SmartObject *) receiver
                            withId: (int) id;
+ (NSString *) tr: (NSString *) text;

@property struct RSmartObject *data;

@end

#endif /* SMARTOBJECT_H */

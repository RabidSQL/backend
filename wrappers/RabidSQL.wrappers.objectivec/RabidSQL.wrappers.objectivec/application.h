#ifndef APPLICATION_H
#define APPLICATION_H

#include <Foundation/Foundation.h>

#include "structures.h"

@interface Application : NSObject

+ (void) processEvents;
+ (Message *) getNextMessage;
+ (void) shutdown;

@end

#endif /* APPLICATION_H */

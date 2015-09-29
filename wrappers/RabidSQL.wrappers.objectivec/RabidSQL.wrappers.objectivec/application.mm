#import <Foundation/Foundation.h>

#include "application.h"
#include "object.h"

#include "logic/include/application.h"

@implementation Application

+ (void) processEvents
{
    RabidSQL::Application::processEvents();
}

+ (Message *) getNextMessage
{
    return Object::convert(RabidSQL::Application::getNextMessage());
}

+ (void) shutdown
{
    RabidSQL::Application::shutdown();
}

@end

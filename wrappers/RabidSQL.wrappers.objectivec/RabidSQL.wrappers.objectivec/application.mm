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
    if (RabidSQL::Application::hasMessage()) {

        // Convert and return a message
        return Object::convert(RabidSQL::Application::getNextMessage());
    } else {

        // Return a null pointer
        return nullptr;
    }
}

+ (void) shutdown
{
    RabidSQL::Application::shutdown();
}

@end

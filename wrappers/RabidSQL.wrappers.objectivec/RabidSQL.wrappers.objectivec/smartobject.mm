#import <Foundation/Foundation.h>

#include "logic/include/smartobject.h"
#include "logic/include/variant.h"

#include "object.h"
#include "prototypes.h"
#include "smartobject.h"
#include "smartobjectwrapper.h"
#include "variant.h"

@implementation SmartObject

- (id) init
{
    self = [super init];

    if (self) {

        _data = new RSmartObject();
        _data->object = new SmartObjectWrapper(self);
    }

    return self;
}

- (void) queueData: (int) id withArguments: (NSMutableArray *) arguments
{
    RabidSQL::VariantVector vector;

    for (Variant *argument in arguments) {

        vector.push_back(Object::unwrap(argument));
    }

    _data->object->queueData(id, vector);
}

- (void) processQueueItem: (int) id withArguments: (NSMutableArray *) arguments
{
}

- (void) connectQueue: (int) id
         withReceiver: (SmartObject *) receiver
{
    _data->object->connectQueue(id, receiver->_data->object);
}

- (void) disconnectQueue
{
    _data->object->disconnectQueue(0);
}

- (void) disconnectQueueById: (int) id
{
    _data->object->disconnectQueue(id);
}

- (void) disconnectQueueByReceiver: (SmartObject *) receiver
{
    _data->object->disconnectQueue(receiver->_data->object);
}

- (void) disconnectQueueByReceiver: (SmartObject *) receiver
                            withId: (int) id
{
    _data->object->disconnectQueue(receiver->_data->object, id);
}

- (void) dealloc
{
    delete _data->object;
    delete _data;
}

+ (NSString *) tr: (NSString *) text;
{
    return text;
}

@end

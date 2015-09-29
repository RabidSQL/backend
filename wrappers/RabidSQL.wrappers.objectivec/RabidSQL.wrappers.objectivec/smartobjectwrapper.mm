#import <Foundation/Foundation.h>

#include "object.h"
#include "smartobject.h"
#include "smartobjectwrapper.h"

#include "logic/include/smartobject.h"

SmartObjectWrapper::SmartObjectWrapper(SmartObjectPtr parent)
{
    this->parent = parent;
}

void SmartObjectWrapper::queueData(int id,
                                   const RabidSQL::VariantVector &arguments)
{
    RabidSQL::SmartObject::queueData(id, arguments);
}

void SmartObjectWrapper::processQueueItem(int id,
                                          RabidSQL::VariantVector arguments)
{
    NSMutableArray *args = [[NSMutableArray alloc] init];

    for (auto it = arguments.begin(); it != arguments.end(); ++it) {

        [args addObject:(Object::wrap(*it))];
    }

    [this->parent processQueueItem: id withArguments: args];
}

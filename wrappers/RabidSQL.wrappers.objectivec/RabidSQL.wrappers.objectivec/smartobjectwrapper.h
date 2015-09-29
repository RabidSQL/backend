#ifndef SMARTOBJECTWRAPPER_H
#define SMARTOBJECTWRAPPER_H

#import <Foundation/Foundation.h>

#include "logic/include/smartobject.h"

#ifdef __OBJC__
@class SmartObject;
typedef SmartObject * SmartObjectPtr;
#else
typedef void * SmartObjectPtr;
#endif

class SmartObjectWrapper: public RabidSQL::SmartObject {

public:
    SmartObjectWrapper(SmartObjectPtr parent);
    void queueData(int id, const RabidSQL::VariantVector &arguments);
    SmartObjectPtr parent;

protected:
    void processQueueItem(int id, RabidSQL::VariantVector arguments);
};

#endif /* SMARTOBJECTWRAPPER_H */

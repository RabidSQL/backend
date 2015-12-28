#ifndef RABIDSQL_MOCKSMARTOBJECT_H
#define RABIDSQL_MOCKSMARTOBJECT_H

#include "SmartObject.h"
#include <vector>

#include "gmock/gmock.h"

namespace RabidSQL {

class MockSmartObject : public SmartObject {
public:
    MockSmartObject(SmartObject *parent = nullptr) {}
    MOCK_METHOD1(contains, bool(SmartObject *));
    MOCK_METHOD2(queueData, void(int, const VariantVector &));
    MOCK_METHOD2(processQueueItem, void(int, VariantVector));
};

} // namespace RabidSQL

#endif //RABIDSQL_MOCKSMARTOBJECT_H

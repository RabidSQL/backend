#ifndef RABIDSQL_SMARTOBJECTTESTER_H
#define RABIDSQL_SMARTOBJECTTESTER_H

#include "SmartObject.h"
#include <vector>

namespace RabidSQL {

class SmartObjectTester : public SmartObject {
public:
    SmartObjectTester(SmartObject *parent = nullptr);
    static bool contains(SmartObjectTester *value);
    static void reset();
    void queueData(int id, const VariantVector &arguments);
    void processQueueItem(const int id, const VariantVector &arguments);
    ~SmartObjectTester();

    std::map<int, VariantVector> data;
    static std::vector<SmartObjectTester *> objects;
};

} // namespace RabidSQL

#endif //RABIDSQL_SMARTOBJECTTESTER_H

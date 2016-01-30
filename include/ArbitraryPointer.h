#ifndef RABIDSQL_ARBITRARYPOINTER_H
#define RABIDSQL_ARBITRARYPOINTER_H

#include "SmartObject.h"

namespace RabidSQL {

class SmartObject;
class ArbitraryPointer : public SmartObject {
public:
    ArbitraryPointer(SmartObject *parent = 0) : SmartObject(parent) {}
    virtual ~ArbitraryPointer() {}
    virtual ArbitraryPointer *clone() = 0;
};

} // namespace RabidSQL

#endif //RABIDSQL_ARBITRARYPOINTER_H

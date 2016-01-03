#ifndef RABIDSQL_TRACKEDPOINTER_H
#define RABIDSQL_TRACKEDPOINTER_H

#include "ArbitraryPointer.h"

namespace RabidSQL {

class TrackedPointer : public ArbitraryPointer
{
public:
    static int count;

    TrackedPointer()
    {
        TrackedPointer::count++;
    }

    virtual ArbitraryPointer *clone()
    {
        return new TrackedPointer();
    }

    virtual ~TrackedPointer()
    {
        TrackedPointer::count--;
    }
};

} // namespace RabidSQL

#endif //RABIDSQL_TRACKEDPOINTER_H

#ifndef RABIDSQL_TRACKEDPOINTER_H
#define RABIDSQL_TRACKEDPOINTER_H

namespace RabidSQL {

class TrackedPointer
{
public:
    static int count;

    TrackedPointer()
    {
        TrackedPointer::count++;
    }

    ~TrackedPointer()
    {
        TrackedPointer::count--;
    }
};

} // namespace RabidSQL

#endif //RABIDSQL_TRACKEDPOINTER_H

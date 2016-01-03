#ifndef RABIDSQL_ARBITRARYPOINTER_H
#define RABIDSQL_ARBITRARYPOINTER_H

namespace RabidSQL {

class ArbitraryPointer {
public:
    virtual ~ArbitraryPointer() {};
    virtual ArbitraryPointer *clone() = 0;
};

} // namespace RabidSQL

#endif //RABIDSQL_ARBITRARYPOINTER_H

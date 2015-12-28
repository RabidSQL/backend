#ifndef RABIDSQL_MESSAGE_H
#define RABIDSQL_MESSAGE_H

#include "NSEnums.h"
#include "Variant.h"

namespace RabidSQL {

class Message {
public:
    MessageType type = NO_MESSAGE;
    std::string label;
    Variant data;

    bool operator!() const;
};

} // namespace RabidSQL

#endif //RABIDSQL_MESSAGE_H

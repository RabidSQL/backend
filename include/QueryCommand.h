#ifndef RABIDSQL_QUERYCOMMAND_H
#define RABIDSQL_QUERYCOMMAND_H

#include "NSEnums.h"
#include "Variant.h"

namespace RabidSQL {

struct QueryCommand {
    Variant uid;
    QueryEvent event;
    VariantVector arguments;
};

} // namespace RabidSQL

#endif //RABIDSQL_QUERYCOMMAND_H

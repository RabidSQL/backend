#ifndef RABIDSQL_QUERYERROR_H
#define RABIDSQL_QUERYERROR_H

#include "Variant.h"

#include <string>

namespace RabidSQL {

struct QueryError {
    bool isError = false;
    Variant code = "";
    std::string string = "";
};

} // namespace RabidSQL

#endif //RABIDSQL_QUERYERROR_H

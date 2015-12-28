#ifndef RABIDSQL_QUERYRESULT_H
#define RABIDSQL_QUERYRESULT_H

#include <list>
#include "NSEnums.h"
#include "Variant.h"
#include "QueryError.h"

namespace RabidSQL {

struct QueryResult {
    Variant uid = "";
    bool is_valid = false;
    int affected_rows = 0;
    int num_rows = 0;
    QueryEvent event;
    QueryError error = QueryError();
    std::list<std::string> columns = std::list<std::string>();
    std::list<VariantVector> rows = std::list<VariantVector>();
};

} // namespace RabidSQL

#endif //RABIDSQL_QUERYRESULT_H

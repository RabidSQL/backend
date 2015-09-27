#ifndef RABIDSQL_STRUCTS_H
#define RABIDSQL_STRUCTS_H

#include "variant.h"

#include <list>

namespace RabidSQL {

#include "enums.h"

struct QueryError {
    bool isError = false;
    Variant code = "";
    std::string string = "";
};

struct SettingsField {
    std::string name;
    std::string label;
    std::string tooltip;
    int width = 100;
};

class Message {
public:
    MessageType type = NO_MESSAGE;
    std::string label;
    Variant data;

    bool operator!() const
    {
        return type == MessageType::NO_MESSAGE;
    }
};

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

struct QueryCommand {
    Variant uid;
    QueryEvent event;
    VariantVector arguments;
};

} // namespace RabidSQL

#endif // RABIDSQL_STRUCTS_H


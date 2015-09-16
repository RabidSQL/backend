#ifndef RABIDSQL_STRUCTS_H
#define RABIDSQL_STRUCTS_H

#include "variant.h"

#include <fstream>
#include <list>

namespace RabidSQL {

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

class MessageType {
public:
    typedef enum {
        _NONE,
        WARNING,
        ERROR,
        CRITICAL,
    } type;
};

class Message {
public:
    MessageType::type type = MessageType::_NONE;
    std::string label;
    Variant data;

    bool operator!() const
    {
        return type == MessageType::_NONE;
    }
};

class QueryEvent {
public:
    typedef enum {
        NONE,
        TEST_CONNECTION,
        LIST_DATABASES,
        LIST_TABLES,
        EXECUTE_QUERY,
        KILL_QUERY,
        DISCONNECT,
        CLEAN_STATE,
        SELECT_DATABASE,
    } type;
};

class FileFormat {
public:
    typedef enum {
        JSON,
        BINARY,
    } format;
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
    QueryEvent::type event;
    VariantVector arguments;
};

// std::fstream "aliases" so different formats can easily be detected
class JsonStream : public std::fstream {};
class BinaryStream : public std::fstream {};

} // namespace RabidSQL

#endif // RABIDSQL_STRUCTS_H


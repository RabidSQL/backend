#ifndef RABIDSQL_ENUMS_H
#define RABIDSQL_ENUMS_H

typedef enum {
    NO_MESSAGE,
    WARNING,
    ERROR,
    CRITICAL,
} MessageType;

typedef enum {
    NO_EVENT,
    TEST_CONNECTION,
    LIST_DATABASES,
    LIST_TABLES,
    EXECUTE_QUERY,
    KILL_QUERY,
    DISCONNECT,
    CLEAN_STATE,
    SELECT_DATABASE,
} QueryEvent;

typedef enum {
    JSON,
    BINARY,
} FileFormat;

typedef enum {
    NO_DATA,
    STRING,
    STRINGVECTOR,
    VARIANTVECTOR,
    VARIANTMAP,
    ULONG,
    LONG,
    UINT,
    INT,
    USHORT,
    SHORT,
    QUERYRESULT,
    FLOAT,
    DOUBLE,
    BOOL,
    _FIRST = NO_DATA,
    _LAST = BOOL,
} DataType;

typedef enum {
    INHERIT,
    MYSQL
} ConnectionType;

#endif //RABIDSQL_ENUMS_H

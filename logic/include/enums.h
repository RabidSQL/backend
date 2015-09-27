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
    D_NULL,
    D_STRING,
    D_STRINGVECTOR,
    D_VARIANTVECTOR,
    D_VARIANTMAP,
    D_ULONG,
    D_LONG,
    D_UINT,
    D_INT,
    D_USHORT,
    D_SHORT,
    D_QUERYRESULT,
    D_FLOAT,
    D_DOUBLE,
    D_BOOLEAN,
    _FIRST = D_NULL,
    _LAST = D_BOOLEAN,
} DataType;

typedef enum {
    INHERIT,
    MYSQL
} ConnectionType;

#endif //RABIDSQL_ENUMS_H

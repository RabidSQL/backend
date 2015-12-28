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
    D_NULL = 0,
    D_STRING = 10,
    D_STRINGVECTOR = 20,
    D_VARIANTVECTOR = 30,
    D_VARIANTMAP = 40,
    D_ULONG = 50,
    D_ULONGLONG = 60,
    D_LONG = 70,
    D_LONGLONG = 80,
    D_UINT = 90,
    D_INT = 100,
    D_USHORT = 110,
    D_SHORT = 120,
    D_QUERYRESULT = 130,
    D_FLOAT = 140,
    D_DOUBLE = 150,
    D_BOOLEAN = 160,
    _FIRST = D_NULL,
    _LAST = D_BOOLEAN,
} DataType;

typedef enum {
    INHERIT,
    MYSQL
} ConnectionType;

#endif //RABIDSQL_ENUMS_H

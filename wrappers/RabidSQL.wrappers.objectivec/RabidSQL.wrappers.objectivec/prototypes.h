#ifndef PROTOTYPES_H
#define PROTOTYPES_H

namespace RabidSQL {

    class ConnectionSettings;
    class DatabaseConnection;
    class DatabaseConnectionManager;
} // namespace RabidSQL

class SmartObjectWrapper;

#include "logic/include/variant.h"

struct RConnectionSettings {
    RabidSQL::ConnectionSettings *object;
};

struct RDatabaseConnection {
    RabidSQL::DatabaseConnection *object;
};

struct RDatabaseConnectionManager {
    RabidSQL::DatabaseConnectionManager *object;
};

struct RSmartObject {
    SmartObjectWrapper *object;
};

struct RVariant
{
    RabidSQL::Variant object;
};

#endif /* PROTOTYPES_H */

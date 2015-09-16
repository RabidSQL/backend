#ifndef RABIDSQL_DATABASECONNECTION_H
#define RABIDSQL_DATABASECONNECTION_H

#include "smartobject.h"
#include "structs.h"
#include "thread.h"

#include <queue>
#include <thread>

namespace RabidSQL {

class DatabaseConnectionManager;
class ConnectionSettings;
class DatabaseConnection: virtual public Thread
{
    friend class DatabaseConnectionManager;
public:
    static const int EXECUTED;

    DatabaseConnection(ConnectionSettings *settings);
    DatabaseConnection(DatabaseConnection *mainConnection,
                       DatabaseConnectionManager *manager);
    virtual void disconnect() = 0;
    virtual QueryResult connect() = 0;
    virtual QueryResult execute(VariantList arguments) = 0;
    virtual QueryResult getDatabases(
            std::vector<std::string> filter = std::vector<std::string>()) = 0;
    virtual QueryResult getTables(std::string database) = 0;
    virtual QueryResult selectDatabase(std::string database) = 0;
    virtual QueryResult killQuery(std::string uuid) = 0;
    virtual DatabaseConnection *clone(DatabaseConnectionManager *manager) = 0;
    virtual ~DatabaseConnection();

    std::mutex mutex;

protected:
    DatabaseConnectionManager *manager;

    void call(Variant uuid, QueryEvent::type event,
              VariantList arguments=VariantList());
    void run();
    DatabaseConnection *getDatabaseConnection(std::string uuid);

private:
    std::queue<QueryCommand> commands;
    DatabaseConnection *mainConnection;
    bool busy;
};

} // namespace RabidSQL

#endif // RABIDSQL_DATABASECONNECTION_H

#include "app.h"
#include "databaseconnection.h"
#include "databaseconnectionmanager.h"

#include <chrono>

namespace RabidSQL {

/**
 *
 * Constructs the database class, applying settings from the provided connection
 * settings
 *
 * @param ConnectionSettings The connection settings to use
 */
DatabaseConnection::DatabaseConnection(ConnectionSettings *settings): Thread()
{
    #ifdef TRACK_POINTERS
    rDebug << "DatabaseConnection::construct" << this;
    #endif

    mainConnection = nullptr;
    manager = nullptr;
}

/**
 *
 * Constructs the database class, taking necessary data from the parent
 *
 * @param mainConnection The parent database connection
 * @param manager The connection manager to associate
 * @return void
 */
DatabaseConnection::DatabaseConnection(DatabaseConnection *mainConnection,
                                       DatabaseConnectionManager *manager):
        Thread()
{
    #ifdef TRACK_POINTERS
    rDebug << "DatabaseConnection::construct" << this;
    #endif

    this->mainConnection = mainConnection;
    this->manager = manager;

    busy = false;
}

/**
 *
 * Gets a database connection identified by uuid. This is here because we cannot
 * dynamically add all of our DatabaseConnection subclasses as friends to the
 * manager.
 *
 * @param uuid The uuid to grab
 *
 * @return A pointer to the database connection
 */
DatabaseConnection *DatabaseConnection::getDatabaseConnection(std::string uuid)
{
    DatabaseConnection *connection;
    connection = manager->getDatabaseConnection(uuid);

    return connection;
}

/**
 *
 * Processes a query
 *
 * @return void
 */
void DatabaseConnection::run()
{
    QueryCommand command;
    QueryResult result;

    result = connect();
    if (result.error.isError) {
        queueData(EXECUTED, VariantList()
                            << command.uid
                            << command.event
                            << result);
        disconnect();

        // Abort
        return;
    }

    while (!isStopping()) {

        // Lock mutex
        mutex.lock();

        if (!commands.empty()) {

            // Get the next command on the queue.
            command = commands.front();

            // Remove from queue
            commands.pop();

            busy = true;
        } else {

            // There's no commands right now. Mark this thread is not busy
            busy = false;
            command.event = QueryEvent::NONE;
        }

        // Unlock mutex
        mutex.unlock();

        switch (command.event) {
        case QueryEvent::DISCONNECT:
            disconnect();
                queueData(EXECUTED, VariantList()
                                    << command.uid
                                    << command.event
                                    << result);
            break;
        case QueryEvent::TEST_CONNECTION:
            if (!result.error.isError) {
                queueData(EXECUTED, VariantList()
                                    << command.uid
                                    << command.event
                                    << result);
            }
            break;
        case QueryEvent::LIST_DATABASES:
            queueData(EXECUTED, VariantList()
                                << command.uid
                                << command.event
                                << getDatabases());
            break;
        case QueryEvent::LIST_TABLES:
            queueData(EXECUTED, VariantList()
                                << command.uid
                                << command.event
                                <<
                                getTables(command.arguments.front().toString()));
            break;
        case QueryEvent::EXECUTE_QUERY:
            queueData(EXECUTED, VariantList()
                                << command.uid
                                << command.event
                                << execute(command.arguments));
            break;
        case QueryEvent::SELECT_DATABASE:
            queueData(EXECUTED, VariantList()
                                << command.uid
                                << command.event
                                << selectDatabase(
                    command.arguments.front().toString()));
            break;
        case QueryEvent::KILL_QUERY:
            queueData(EXECUTED, VariantList()
                                << command.uid
                                << command.event
                                <<
                                killQuery(command.arguments.front().toString()));
            break;
        case QueryEvent::CLEAN_STATE:
            // @TODO: Finish up any current transactions if applicable
            break;
        case QueryEvent::NONE:
            // Sleep for 100ms. We don't want to pin the cpu on nothingness
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            break;
        }
    }

    #ifdef DEBUG
    rDebug << "Finished loop";
    #endif

    // Disconnect if we're connected
    disconnect();

    #ifdef DEBUG
    rDebug << "End of thread execution";
    #endif
}

/**
 *
 * Sets this connection's event type and arguments
 *
 * @param Variant the uid to use
 * @param QueryEvent event The event type to set
 * @param arguments The arguments to use
 * @return void
 */
void DatabaseConnection::call(Variant uid, QueryEvent::type event,
                               VariantList arguments)
{
    QueryCommand command;

    // Configure command
    command.uid = uid;
    command.event = event;
    command.arguments = arguments;

    // Lock mutex
    mutex.lock();

    // Set variables. This is also done in the thread loop, but we want this
    // to be known as busy immediately, so even if the very next statement
    // needs a thread, it won't re-use this one.
    this->busy = true;

    // Add to queue
    this->commands.push(command);

    // Unlock mutex
    mutex.unlock();
}

/**
 *
 * Kills the current query
 *
 * @param uuid The connection/query identifier to kill
 * @return void
 */
QueryResult DatabaseConnection::killQuery(std::string uuid)
{
#ifdef DEBUG
    rDebug << "Base kill called. Not implemented";
#endif
    // @TODO: either abstract this entirely, or add not implemented code for
    // anything that still gets called on the base class
    return QueryResult();
}

/**
 *
 * Destroys the connection, freeing any applicable memory
 *
 * @return void
 */
DatabaseConnection::~DatabaseConnection()
{
    #ifdef TRACK_POINTERS
    rDebug << "DatabaseConnection::destroy" << this;
    #endif
}

const int DatabaseConnection::EXECUTED = 0x1;

} // namespace RabidSQL

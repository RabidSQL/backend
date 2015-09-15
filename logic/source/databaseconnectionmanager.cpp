#include "app.h"
#include "application.h"
#include "connectionsettings.h"
#include "databaseconnection.h"
#include "databaseconnectionmanager.h"
#include "uuid.h"

#include <ctime>
#include <unistd.h>

namespace RabidSQL {

/**
 *
 * Initializes a connection manager, setting the main connection to the provided
 * argument
 *
 * @param mainConnection The parent database connection
 * @param settings The setting to apply
 * @return void
 */
DatabaseConnectionManager::DatabaseConnectionManager(
        DatabaseConnection *mainConnection, ConnectionSettings *settings) :
    SmartObject(0)
{
    #ifdef TRACK_POINTERS
    rDebug << "DatabaseConnectionManager::construct" << this;
    #endif

    type = static_cast<ConnectionSettings::type>(
        settings->get("type").toUInt());
    this->mainConnection = mainConnection;

    maxConnections = settings->get("max_connections").toUInt();

    if (maxConnections == 0) {
        maxConnections = 1;
    }

}

/**
 *
 * Get the type of SQL connection this represents
 *
 * @return The type of sql connection
 */
ConnectionSettings::type DatabaseConnectionManager::getType()
{
    return type;
}

/**
 *
 * Reserves a database connection. Until this is freed, it will not be re-used.
 *
 * @param timeout The minimum amount of time to use this connection in seconds
 * @param receiver The object which we'll send any data to
 *
 * @return A UUID for this connection
 */
DatabaseConnection *DatabaseConnectionManager::reserveDatabaseConnectionObj(
        int timeout, SmartObject *receiver)
{
    ConnectionRecord record, currentRecord;
    DatabaseConnection *connection = nullptr, *currentConnection;
    int count;
    bool running, busy;

    if (timeout != 0) {

        // Expiry = unixtime + whatever timeout started as
        timeout += std::time(nullptr);
    }

    while (connection == nullptr) {

        // Reset counter
        count = 0;

        // Iterate connections
        Connections connections(this->connections);
        for (Connections::iterator it = connections.begin();
             it != connections.end(); ++it) {

            currentConnection = it->first;
            currentRecord = it->second;

            if (currentRecord.expiry != 0) {

                // This connection has an expiry. No expiry indicates that it is
                // a reserved connection that should not be re-assigned.
                if (connection == nullptr) {

                    // Lock this connection's mutex
                    currentConnection->mutex.lock();

                    // Check if this connection is in the process of shutting
                    // down or is busy with something (or has pending queries)
                    busy = currentConnection->busy
                            || !currentConnection->commands.empty();

                    // Unlock the mutex
                    currentConnection->mutex.unlock();

                    if (currentConnection->isStopping() && !busy) {

                        // Re-use this connection
                        connection = currentConnection;

                        // Disconnect the execution signal so if the old
                        // receiver is still around it won't keep getting
                        // signals
                        connection->disconnectQueue(
                            DatabaseConnection::EXECUTED);

                        // Rollback transaction if applicable
                        connection->call(Variant(),
                                         QueryEvent::CLEAN_STATE);
                    }

                } else if (currentRecord.expiry != 0
                           && currentRecord.expiry > std::time(nullptr)) {

                    // This connection has expired

                    // Lock this connection's mutex
                    currentConnection->mutex.lock();

                    // Check if this connection is in the process of shutting
                    // down or is busy with something
                    busy = currentConnection->busy
                            || !currentConnection->commands.empty();

                    // Unlock the mutex
                    currentConnection->mutex.unlock();

                    if (!busy) {
                        // Tell the connection to disconnect. We'll free memory
                        // after that is finished
                        if (!currentConnection->isStopping()) {

                            // Ensure this connection's signals are disconnected
                            currentConnection->disconnectQueue(
                                DatabaseConnection::EXECUTED);

                            // Connect to ourself. After the disconnect
                            // completes, we need to free the connection

                            currentConnection->connectQueue(
                                DatabaseConnection::EXECUTED, this);

                            currentConnection->call(Variant(currentRecord.uuid),
                                QueryEvent::DISCONNECT);
                        }

                        // Remove from the hash
                        this->connections.erase(currentConnection);

                        // Add to the disconnections hash
                        disconnectingConnections[currentRecord.uuid]
                                = currentConnection;
                    }
                }

                count++;
            }
        }

        if (connection == nullptr && count < maxConnections) {

            // Initialize new connection
            connection = mainConnection->clone(this);

            // Start new thread
            connection->start();
        }

        if (connection != nullptr) {
            record.expiry = timeout;
            record.uuid = UUID::makeUUID();
            record.receiver = receiver;
            this->connections[connection] = record;
        }

        if (connection == nullptr) {
            // Process any pending events
            Application::processEvents();

            // Also sleep for 30ms. There may not be events to process above
            // and we don't want to pin the cpu
            usleep(30 * 1000);
        }
    }

    if (receiver != nullptr) {

        // Lock mutex
        connection->mutex.lock();

        // Connect signal
        connection->connectQueue(DatabaseConnection::EXECUTED, receiver);

        // Unlock mutex
        connection->mutex.unlock();
    }

    return connection;
}

/**
 *
 * We received a disconnect signal. Lets cleanup the connection in question
 *
 * @param args The arguments passed from the database
 * @return void
 */
void DatabaseConnectionManager::disconnected(const VariantList &args)
{
    DatabaseConnection *connection;
    std::string uuid;
    Variant uid = args.data()[0];

    uuid = uid.toString();

    // Find connection
    connection = disconnectingConnections[uuid];

    // Stop running
    connection->stop();

    // Wait for the connection to finish
    while (!connection->isFinished()) {

        // Process any pending events
        Application::processEvents();

        // Sleep for 30ms.
        usleep(30 * 1000);
    }

    // Remove from collection
    disconnectingConnections.erase(uuid);

    // Free memory
    delete connection;
}

/**
 *
 * Reserves a database connection. Until this is freed, it will not be re-used.
 *
 * @param expiry The length of time in seconds before this connection expires
 * @param receiver The object that will receieve signals emitted by this db
 *
 * @return A UUID for this connection
 */
std::string DatabaseConnectionManager::reserveDatabaseConnection(int expiry,
        SmartObject *receiver)
{
    DatabaseConnection *connection;

    // Reserve connection
    connection = reserveDatabaseConnectionObj(expiry, receiver);

    // Return UUID
    return connections[connection].uuid;
}

/**
 *
 * Released the connection identified by UUID. If it is busy, we'll wait for it
 * to finish
 *
 * @param uuid The uuid identifying this connection
 * @return void
 */
void DatabaseConnectionManager::releaseDatabaseConnection(std::string uuid)
{
    DatabaseConnection *connection = nullptr;

    // Iterate connections
    for (Connections::const_iterator it = connections.begin();
            it != connections.end(); ++it) {
        if (it->second.uuid == uuid) {

            // Stop once we find the connection
            connection = it->first;
            break;
        }
    }

    // Reset expiry time. Note that reserveDatabaseConnection will still not
    // return this connection until any queries it is executing have completed.

    // Set expiry
    connections[connection].expiry = std::time(nullptr)
         + DatabaseConnectionManager::DEFAULT_EXPIRY;

}

/**
 *
 * Calls a specific SQL query on the connection
 *
 * @param connection The connection
 * @param uid The uid to associate with this query
 * @param event The event to execute
 * @param arguments Any necessary arguments
 *
 * @return void
 */
void DatabaseConnectionManager::call(DatabaseConnection *connection,
                                     Variant uid,
                                     QueryEvent::type event,
                                     VariantList arguments)
{
    connection->call(uid, event, arguments);
}

/**
 *
 * Calls a specific SQL query on the connection identified by uuid
 *
 * @param uuid The uuid of the connection
 * @param uid The uid of the query
 * @param event The event to execute
 * @param arguments Any necessary arguments
 * @return void
 */
void DatabaseConnectionManager::call(std::string uuid, Variant uid,
                                     QueryEvent::type event,
                                     VariantList arguments)
{
    for (Connections::const_iterator it = connections.begin();
            it != connections.end(); ++it) {
        if (it->second.uuid == uuid) {
            call(it->first, uid, event, arguments);
            return;
        }
    }
}

/**
 *
 * Gets a database connection identified by uuid
 *
 * @param uuid The uuid to grab
 * @return A pointer to the database connection
 */
DatabaseConnection *DatabaseConnectionManager::getDatabaseConnection(
        std::string uuid)
{
    for (Connections::const_iterator it = connections.begin();
            it != connections.end(); ++it) {
        if (it->second.uuid == uuid) {
            return it->first;
        }
    }

    return nullptr;
}

/**
 *
 * Kills a query.
 *
 * @param uuid The uuid of the connection
 * @return void
 */
void DatabaseConnectionManager::killQuery(std::string uuid)
{
    DatabaseConnection *killingConnection;
    ConnectionRecord record;

    for (Connections::const_iterator it = connections.begin();
            it != connections.end(); ++it) {

        record = it->second;

        if (record.uuid == uuid) {

            // Reserve a database connection
            killingConnection = reserveDatabaseConnectionObj(DEFAULT_EXPIRY,
                                                             record.receiver);

            // Kill query
            call(killingConnection, Variant(), QueryEvent::KILL_QUERY,
                 VariantList() << uuid);

            // Release connection, for sanity. Because there is alrady an
            // expiry, it will be auto-freed regardless.
            releaseDatabaseConnection(connections[killingConnection].uuid);

            // No need to look at any other connections
            break;
        }
    }
}

/**
 *
 * Destroys this connection manager
 *
 * @return void
 */
DatabaseConnectionManager::~DatabaseConnectionManager()
{
    DatabaseConnection *connection;
    bool waiting = true;

    while (waiting) {

        // Our default state is not waiting
        waiting = false;

        Connections connections(this->connections);
        for (Connections::const_iterator it = connections.begin();
                it != connections.end(); ++it) {

            connection = it->first;

            if (connection->isStopping()) {

                // Wait for the connection to finish
                connection->join();

                // Remove from collection
                this->connections.erase(connection);

                // Free memory
                delete connection;
            } else {

                // Ask the connection to stop running
                connection->stop(false);

                // This connection may not be finished yet
                waiting = true;
            }
        }
    }

    delete this->mainConnection;

    #ifdef TRACK_POINTERS
    rDebug << "DatabaseConnectionManager::destroy" << this;
    #endif
}

} // namespace RabidSQL


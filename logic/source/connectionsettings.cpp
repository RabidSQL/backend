#include "app.h"
#include "application.h"
#include "connectionsettings.h"
#include "filestream.h"
#include "structs.h"
#include "uuid.h"

#include <fstream>

namespace RabidSQL {

/**
 *
 * Construct the ConnectionSettings class
 *
 * @param parent The parent settings, if applicable
 * @return void
 */
ConnectionSettings::ConnectionSettings(ConnectionSettings *parent) :
    SmartObject(parent)
{
    if (parent != nullptr) {
        set("parent", parent->get("uuid"));
    } else {
        remove("parent");
    }
}

/**
 *
 * Returns our parent object. This is just a helper function so we don't have
 * to re-cast outside of the class every time we need to get the parent.
 *
 * @return The parent of these settings
 */
ConnectionSettings *ConnectionSettings::getParent()
{
    return (ConnectionSettings *) SmartObject::getParent();
}

/**
 *
 * Sets the parent ConnectionSettings object
 *
 * @param parent The parent to set to, or nullptr to not set a parent
 * @return void
 */
void ConnectionSettings::setParent(ConnectionSettings *parent)
{
    SmartObject::setParent(parent);

    if (parent != nullptr) {
        set("parent", parent->get("uuid"));
    } else {
        remove("parent");
    }
}

/**
 *
 * Returns true if the given key is associated with a setting. Else false.
 *
 * @param key The key to check
 * @return True if the given key is associated with a settings
 */
bool ConnectionSettings::contains(std::string key)
{
    return settings.find(key) != settings.end();
}

/**
 *
 * Gets a setting identified by key. If it doesn't exist, we'll recursively try
 * each parent until we hit the top level. If this happens we'll return null.
 *
 * @param key The (std::string) key to lookup
 * @param bubble true to bubble up to parents if we don't have the given setting
 * @return The result, or nullptr if there was none
 */
Variant ConnectionSettings::get(std::string key, bool bubble)
{
    ConnectionSettings *parent;

    // We've got this key, send it back!
    if (contains(key)) {
        return settings[key];
    }

    // uuid and name are handled specially. UUID needs to be generated if it
    // is missing and name mustn't bubble.
    if (key == "uuid") {

        // Generate UUID
        settings["uuid"] = UUID::makeUUID();

        return settings["uuid"];
    } else if (key == "name" or key == "parent") {

        // Name and parent should not climb the tree
        bubble = false;
    }

    // If bubble is true, we need to look for the first parent that has this
    // setting set.
    if (bubble && (parent = (ConnectionSettings*) this->getParent()) != nullptr) {
        return parent->get(key, true);
    }

    // Couldn't find it :(
    return Variant();
}

/**
 *
 * Sets the setting for key to value
 *
 * @param key The key to add or replace
 * @param value The value to set
 * @return void
 */
void ConnectionSettings::set(std::string key, const Variant value)
{
    settings[key] = value;
}

/**
 *
 * Removes a setting identified by key
 *
 * @param key The key to remove
 * @return void
 */
void ConnectionSettings::remove(std::string key)
{
    Settings::iterator it = settings.find(key);
    if (it != settings.end()) {

        // Remove from map
        settings.erase(it);
    }
}

/**
 *
 * Loads all connection settings from a configuration file formatted as format
 *
 * @param format The format of the file
 * @param filename The filename to load
 * @return An array of connection settings pointers
 */
std::vector<ConnectionSettings *> ConnectionSettings::load(
        FileFormat::format format, std::string filename)
{
    std::vector<ConnectionSettings *> connectionList;
    ConnectionSettings *connectionSettings;
    FileStream *stream;
    Variant data;

    switch (format) {
    case FileFormat::BINARY:

        stream = new BinaryStream();
        break;
    case FileFormat::JSON:

        stream = new JsonStream();
        break;
    }

    // Open the file
    if (!stream->open(filename, std::ios::in)) {

        // Failed to open file
        return connectionList;
    }

    // Load stream
    *stream >> data;

    if (data.getType() == Variant::VARIANTVECTOR) {

        auto connections = data.toVariantVector();

        for (auto it = connections.begin(); it != connections.end(); ++it) {

            if (it->getType() == Variant::VARIANTMAP) {
                // Initialize settings
                connectionSettings = new ConnectionSettings();

                // Load from map
                *connectionSettings << it->toVariantMap();

                // Add to collection
                connectionList.push_back(connectionSettings);

                // Reparent the children and add to the connectionList
                // connection
                reparentChildren(connectionList, connectionSettings);
            }
        }
    }

    // Close stream
    stream->close();

    // Free memory
    delete stream;

    if (connectionList.size() == 0) {

        // Add default group
        connectionSettings = new ConnectionSettings();
        connectionSettings->set("name", "Default");
        connectionSettings->set("type", MYSQL);
        connectionList.push_back(connectionSettings);
    }

    return connectionList;
}

/**
 *
 * Recursively finds children in the connection list and re-parents them
 *
 * @return void
 */
void ConnectionSettings::reparentChildren(
        std::vector<ConnectionSettings *> &connectionList,
        ConnectionSettings *currentSettings)
{
    if (!currentSettings->contains("children")) {

        // This connection has no children. Look no further.
        return;
    }

    // Get children for the current connection settings
    auto children = currentSettings->get("children").toVariantVector();

    // Iterate collection
    for (auto it = children.begin(); it != children.end(); ++it) {

        // Initialize settings
        auto connectionSettings = new ConnectionSettings();

        // Load from map
        *connectionSettings << it->toVariantMap();

        // Set the parent for this connection to the correct parent
        connectionSettings->setParent(currentSettings);

        // Add to collection
        connectionList.push_back(connectionSettings);

        // Extract children
        reparentChildren(connectionList, connectionSettings);
    }
}

/**
 *
 * Saves connection settings given an array of settings. This also destroys
 * their pointers if destroy is true
 *
 * @param settings An array of connection settings
 * @param format The file format to use
 * @param filename The filename to save to
 * @return void
 */
void ConnectionSettings::save(
        std::vector<ConnectionSettings *> &settings, FileFormat::format format,
        std::string filename)
{
    VariantVector connections;
    FileStream *stream;

    for (auto it = settings.cbegin(); it != settings.cend(); ++it) {

        auto connection = (*it);

        if (connection->getParent() == nullptr) {

            // Add to collection
            connections.push_back(connection->toVariantMap());
        }
    }

    switch (format) {
        case FileFormat::BINARY:

            stream = new BinaryStream();
            break;
        case FileFormat::JSON:

            stream = new JsonStream();
    }

    // Open stream
    stream->open(filename, std::ios::out);

    // Save connections
    *stream << connections;

    // Close stream
    stream->close();

    // Free memory
    delete stream;
}

VariantMap ConnectionSettings::toVariantMap()
{
    VariantMap map;

    // Ensure UUID is generated already in the event this is a new
    // connection
    get("uuid");

    for (auto it = settings.begin(); it != settings.end(); ++it) {

        if (it->first == "parent") {

            // Do not store parent
            continue;
        }

        // Add to variant map
        map[it->first] = it->second;
    }

    // Get children
    auto children = getChildren(this);

    if (children.size() > 0) {

        // Add children
        map["children"] = children;
    }

    return map;
}

VariantVector ConnectionSettings::getChildren(ConnectionSettings *parent)
{
    VariantVector vector;

    // Get children
    auto children = SmartObject::getChildren();

    for (auto it = children.begin(); it != children.end(); ++it) {

        auto child = static_cast<ConnectionSettings *>(*it);

        // Add to collection
        vector.push_back(child->toVariantMap());
    }

    return vector;
}

void ConnectionSettings::operator<<(const VariantMap &value)
{
    for (auto it = value.begin(); it != value.end(); ++it) {
        std::string key = it->first;
        Variant value = it->second;

        // Set the key-value pair
        set(key, value);
    }
}

/**
 *
 * Gets the type of settings
 *
 * @return type The type of connection these settings belong to
 */
ConnectionSettings::type ConnectionSettings::getType()
{
    return static_cast<type>(get("type").toUInt());
}

/**
 *
 * Destructor for the class
 */
ConnectionSettings::~ConnectionSettings()
{
}

} // namespace RabidSQL

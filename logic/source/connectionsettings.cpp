#include "app.h"
#include "application.h"
#include "connectionsettings.h"
#include "structs.h"
#include "uuid.h"

#include <fstream>
#include <iostream>

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
    Variant parent, uid;

    switch (format) {
    case FileFormat::BINARY:

        // Load from a binary file
        connectionList = loadBinary(filename);
        break;
    case FileFormat::JSON:

        // Load from a JSON file
        connectionList = loadJson(filename);
        break;
    }

    if (connectionList.size() == 0) {

        // Add default group
        connectionSettings = new ConnectionSettings();
        connectionSettings->set("name", "Default");
        connectionSettings->set("type", MYSQL);
        connectionList.push_back(connectionSettings);
    } else {

        // @TODO: Configure tree hierarchy
    }

    return connectionList;
}

/**
 *
 * Loads all connection settings from a configuration file formatted as binary
 *
 * @param filename The filename to load
 * @return An array of connection settings pointers
 */
std::vector<ConnectionSettings *> ConnectionSettings::loadBinary(
        std::string filename)
{
    std::vector<ConnectionSettings *> connectionList;
    ConnectionSettings *connectionSettings;
    VariantVector settings;
    int size;

    BinaryStream stream;

    // Open the file
    stream.open(filename, std::ios::binary | std::ios::in);

    if (!stream.is_open()) {

        // Failed to open stream
        return connectionList;
    }

    if (!stream.eof()) {

        char format[7];
        // Check format
        stream.read(format, 6);

        // Ensure null-termination
        format[6] = 0;

        if (std::string(format) != "RSQAF0") {

            // This file is improperly formatted
            return connectionList;
        }
    }

    // Read file
    while (!stream.eof()) {

        char marker[4];

        // Check for marker
        stream.read(marker, 3);

        // Ensure null-termination
        marker[3] = 0;

        if (std::string(marker) != "SOL" || stream.eof()) {

            // Stop reading
            break;
        }

        // Read a list of variants
        settings << stream;

        // Ensure there are an even number of elements (key, value)
        size = settings.size();
        assert(size % 2 == 0);

        // The file is corrupt or there are no connections
        if (size == 0 || size % 2 == 1) {

            // Iterate to find any connections we've already created
            for (auto it = connectionList.begin(); it != connectionList.end();
                    ++it) {

                // Free memory
                delete *it;
            }

            // Erase settings from the list
            connectionList.clear();

            // Return our empty list
            return connectionList;
        }
        // This is valid, lets load it into a settings object
        connectionSettings = new ConnectionSettings();
        *connectionSettings << settings;

        connectionList.push_back(connectionSettings);
    }

    // Close file
    stream.close();

    return connectionList;
}

/**
 *
 * Loads all connection settings from a configuration file formatted as JSON
 *
 * @param filename The filename to load
 * @return An array of connection settings pointers
 */
std::vector<ConnectionSettings *> ConnectionSettings::loadJson(
        std::string filename)
{
    #ifdef DEBUG
    rDebug << "ConnectionSettings::loadJson not implemented!";
    #endif
    return std::vector<ConnectionSettings *>();
}

/**
 *
 * Saves connection settings given an array of settings. This also destroys
 * their pointers if destroy is true
 *
 * @param settings An array of connection settings
 * @param format The file format to use
 * @param filename The filename to save to
 * @param destroy A boolean indicating whether or not to destroy the settings
 * after saving
 * @return void
 */
void ConnectionSettings::save(
        std::vector<ConnectionSettings *> &settings, FileFormat::format format,
        std::string filename, bool destroy)
{

    switch (format) {
        case FileFormat::BINARY:

            // Load from a binary file
            saveBinary(settings, filename);
            break;
        case FileFormat::JSON:

            // Load from a JSON file
            saveJson(settings, filename);
            break;
    }

    if (destroy) {

        for (auto it = settings.begin(); it != settings.end(); ++it) {

            // Free memory
            delete *it;
        }

        // Clear settings
        settings.clear();
    }
}

/**
 *
 * Saves connection settings given an array of settings.
 *
 * @param settings An array of connection settings
 * @param filename The filename to save to
 * @return void
 */
void ConnectionSettings::saveBinary(std::vector<ConnectionSettings *> &settings,
        std::string filename)
{
    BinaryStream stream;

    // Open the file
    stream.open(filename, std::ios::binary | std::ios::out);

    stream.write("RSQAF0", 6);

    for (auto it = settings.begin(); it != settings.end(); ++it) {

        Settings settings = (*it)->settings;

        VariantVector variantList;
        for (auto mit = settings.begin(); mit != settings.end(); ++mit) {
            variantList.push_back(mit->first);
            variantList.push_back(mit->second);
        }

        // Write start of list marker. This is so that we know when we're out of
        // variants and don't end up with an extra blank one at the end (because
        // a blank variant is valid so there is no way to tell if it should be
        // there or not unless we make it a fatal error in the VariantVector
        // class)
        stream.write("SOL", 3);
        variantList >> stream;
    }

    // Close file
    stream.close();
}

/**
 *
 * Saves connection settings given an array of settings.
 *
 * @param settings An array of connection settings
 * @param filename The filename to save to
 * @return void
 */
void ConnectionSettings::saveJson(std::vector<ConnectionSettings *> &settings,
                                    std::string filename)
{
    #ifdef DEBUG
    rDebug << "ConnectionSettings::saveJson not implemented!";
    #endif
}

void ConnectionSettings::operator<<(const VariantVector &value)
{
    if (value.size() % 2 == 1) {
        // We've been passed corrupt data!
        #ifdef DEBUG
        rDebug << "Corrupt data (uneven list) passed to ConnectionSettings<<";
        #endif
        return;
    }

    for (auto it = value.begin(); it != value.end(); ++it) {
        std::string key = (*it).toString();
        it++;
        Variant value = *it;

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

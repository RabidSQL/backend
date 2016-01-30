#include "App.h"
#include "SmartObject.h"
#include "Application.h"

namespace RabidSQL {

/**
 *
 * Initializes the SmartObject object, setting the parent to the provided
 * argument
 *
 * @param parent The parent to set
 * @return void
 */
SmartObject::SmartObject(SmartObject *parent)
{
    #ifdef TRACK_POINTERS
    rDebug << "SmartObject::construct" << this;
    #endif

    this->parent = nullptr;
    this->setParent(parent);
    disconnectFromParent = true;
    disconnectFromApplication = true;
    Application::getInstance()->registerObject(this);
}

/**
 *
 * Sets the parent to the provided parent, adding this object to the parent's
 * children if it is non-nullptr
 *
 * @param parent The parent to set
 * @return void
 */
void SmartObject::setParent(SmartObject *parent)
{
    if (this->parent == parent) {

        // Nothing to do, things haven't changed
        return;
    }

    if (this->parent != nullptr) {

        // Remove the old parent
        this->parent->removeChild(this, false);
    }

    this->parent = parent;

    this->parent->addChild(this);

    disconnectFromParent = true;
}

/**
 *
 * Returns the parent of this object
 *
 * @return The parent
 */
SmartObject *SmartObject::getParent()
{
    return parent;
}

/**
 *
 * Returns any children associated with this object
 *
 * @return The vector of children
 */
std::vector<SmartObject *> SmartObject::getChildren()
{
    return children;
}

/**
 *
 * Adds a child to our watched objects
 *
 * @param child The child to add
 * @return void
 */
void SmartObject::addChild(SmartObject *child)
{
    this->children.push_back(child);
}

/**
 *
 * Removes a child from our vector
 *
 * @param child The child to remove
 * @param deallocate Should we de-allocate the child's memory?
 * @return void
 */
void SmartObject::removeChild(SmartObject *child, bool deallocate)
{
    // Iterate through the vector, looking for the child in question
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (*it == child) {
            // Remove from the vector
            this->children.erase(it);

            // Stop iterating
            break;
        }
    }
}

/**
 *
 * Translates text to the current language. Not yet implemented (currently just
 * returns text)
 *
 * @param text The text to translate
 * @return The translated text
 */
std::string SmartObject::tr(std::string text)
{
    return text;
}

/**
 *
 * Connects to receiver's queue. Receiver will be sent data we emit
 *
 * @param id The id of the signal to connect
 * @param receiver The receiving object
 * @return void
 */
void SmartObject::connectQueue(int id, SmartObject *receiver)
{
    assert(receiver != nullptr);

    if (receiver == nullptr) {

        // Abort if the function isn't valid
        return;
    }

    // Lock mutex
    mutex.lock();

    // Add to collection
    connectedObjects.insert(
        std::pair<int, SmartObject *>(id, receiver));

    // Unlock mutex
    mutex.unlock();
}

/**
 *
 * Disconnects any objects identified by id
 *
 * @param id The id of the queue to disconnect
 * @return void
 */
void SmartObject::disconnectQueue(int id)
{
    disconnectQueue(0, id);
}

/**
 *
 * Disconnects any objects identified by receiver and/or id
 *
 * @param receiver The receiver to disconnect
 * @param id The id of the queue to disconnect
 */
void SmartObject::disconnectQueue(const SmartObject *receiver, int id)
{
    // Lock mutex
    mutex.lock();

    if (id != 0) {

        if (receiver == nullptr) {

            // A function wasn't passed, so lets disconnect everything related
            // to this id
            connectedObjects.erase(connectedObjects.find(id));
        } else {
            auto objects = connectedObjects;

            // Copy objects so we don't explode things when we delete while
            // iterating
            auto its = objects.equal_range(id);

            for (auto it = its.first; it != its.second; ++it) {

                if (it->second == receiver) {

                    // Disconnect this
                    connectedObjects.erase(it);
                }
            }
        }
    } else if (receiver != nullptr) {

        // Copy objects so we don't explode things when we delete while
        // iterating
        auto objects = connectedObjects;

        for (auto it = objects.begin(); it != objects.end(); ++it) {

            if (it->second == receiver) {

                // Disconnect this
                connectedObjects.erase(it);
            }
        }
    } else {

        // Disconnect all piipes
        connectedObjects.clear();
    }

    // Unlock mutex
    mutex.unlock();
}

/**
 *
 * Queue some data for other object(s) to pickup
 *
 * @param id The id of the queue
 * @param arguments The arguments to pass
 * @return void
 */
void SmartObject::queueData(int id, const VariantVector &arguments)
{
    // Find all of the connected functions matching this id
    auto its = connectedObjects.equal_range(id);

    // Add to queue(s)
    for (auto it = its.first; it != its.second; ++it) {

        // Lock mutex
        it->second->mutex.lock();

        it->second->dataQueue.push(Data(id, arguments));

        // Unlock mutex
        it->second->mutex.unlock();
    }
}

/**
 *
 * Process any data in the queue, clearing it in our wake. Note that while
 * this is publicly exposed and calling it won't actually hurt anything
 * (unless you run it on the wrong thread, then it potentially could),
 * Application::processQueue should be used in place. It is public only for
 * testing.
 *
 * @return int Returns the id of the last processed event
 */
int SmartObject::processQueue()
{
    bool finished;
    int id = -1;

    do {

        // Lock mutex
        mutex.lock();

        finished = dataQueue.empty();

        if (!finished) {
            auto data = dataQueue.front();
            dataQueue.pop();

            // Unlock mutex
            mutex.unlock();

            id = data.id;
            if (id == DELETE_LATER) {
                finished = true;
            } else {
                processQueueItem(id, data.arguments);
            }
        } else {

            // Unlock mutex
            mutex.unlock();
        }

    } while (!finished);

    return id;
}

/**
 * Stores some arbitrary data locally, identified by key
 *
 * @param std::string key The identifier for the data
 * @param Variant value The data itself
 * @return void
 */
void SmartObject::setArbitraryData(std::string key, const Variant &value)
{
    arbitraryData[key] = value;
}

/**
 * Retreieves some arbitrary data locally, identified by key
 *
 * @param std::string key The identifier for the data
 * @return Variant
 */
Variant SmartObject::getArbitraryData(std::string key)
{
    if (arbitraryData.find(key) == arbitraryData.end()) {
        // This key doesn't exist
        return Variant(nullptr);
    }

    return arbitraryData[key];
}

/**
 *
 * Process a record from the queue. This should be implemented in any subclasses
 * which use the queue
 *
 * @param id The type of queue
 * @param args Any applicable arguments
 * @return void
 */
void SmartObject::processQueueItem(const int, const VariantVector &)
{
    #ifdef DEBUG
    rDebug << "Unimplemented SmartObject::processData is receieving data! T_T";
    #endif
}

/**
 * Destroys this object. Tells the parent that it has been destroyed, and
 * destroys all children
 */
SmartObject::~SmartObject()
{
    #ifdef TRACK_POINTERS
    rDebug << "SmartObject::destruct" << this << "Parent:" << parent;
    #endif

    if (disconnectFromApplication) {

        // Unregister with our application if applicable
        Application::getInstance()->unregisterObject(this);
    }

    if (parent != nullptr && disconnectFromParent) {
        parent->removeChild(this, false);
    }

    // Iterate through the vector, looking for children
    for (auto it = children.begin(); it != children.end(); ++it) {

        // Tell this element to not try to disconnect from us since we're in
        // the process of shutting down
        (*it)->disconnectFromParent = false;

        // Free memory
        delete *it;
    }
}

/**
 * Schedule this object for deletion later on
 */
void SmartObject::deleteLater()
{
    // Lock mutex
    this->mutex.lock();

    this->dataQueue.push(Data(DELETE_LATER, VariantVector()));

    // Unlock mutex
    this->mutex.unlock();
}

/**
 *
 * Initializes a Data object. This just creates a class with the provided
 * queue id and argument list.
 *
 * @return void
 */
SmartObject::Data::Data(int id, const VariantVector &arguments)
{
    this->id = id;
    this->arguments = arguments;
};

} // namespace RabidSQL


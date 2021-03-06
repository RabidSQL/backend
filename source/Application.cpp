#include "Application.h"
#include "SmartObject.h"
#include "Message.h"

namespace RabidSQL {

Application *Application::instance = nullptr;

/**
 *
 * Returns the application singleton
 *
 * @return An instance of Application
 */
Application *Application::getInstance()
{
    if (Application::instance == nullptr) {

        // Initialize application
        new Application();
    }

    return Application::instance;
}

/**
 * Application constructor
 */
Application::Application() {
    if (instance != nullptr) {
        delete instance;
    }

    instance = this;
}

/**
 *
 * Processes any queued events
 *
 * @return void
 */
void Application::processEvents()
{
    auto app = Application::getInstance();
    auto objects = app->objects.get();

    if (objects->size() == 0) {

        // If there are no objects, there is absolutely nothing to do.
        return;
    }

    for (auto it = objects->begin(); it != objects->end(); ++it) {
        auto object = *it;

        // Tell this object to check its queue
        if (object->processQueue() == SmartObject::DELETE_LATER) {
            delete object;

            // Restart iterator
            it = objects->begin();
        }
    }
}

/**
 *
 * Post a message to the event queue
 *
 * @param type The type of message to post
 * @param label A label for the message
 * @param data A variant of any associated data. This should typically be a
 * string, but is a Variant so a StringList can also be used.
 * @return void
 */
void Application::postMessage(MessageType type, std::string label, Variant data)
{
    auto app = Application::getInstance();

    // Initialize message
    Message message;
    message.type = type;
    message.label = label;
    message.data = data;

    // Lock mutex
    app->mutex.lock();

    // Add to collection
    app->messages.push(message);

    // Unlock mutex
    app->mutex.unlock();
}

/**
 *
 * Checks if there are any available messages. Returns true if there are any
 * or false if not
 *
 * @return bool
 */
bool Application::hasMessage()
{
    auto app = Application::getInstance();

    // Lock mutex
    app->mutex.lock();

    bool hasMessages = !app->messages.empty();

    // Unlock mutex
    app->mutex.unlock();

    return hasMessages;
}

/**
 *
 * Returns the next available message
 *
 * @return Message
 */
Message Application::getNextMessage()
{
    auto app = Application::getInstance();

    Message message;

    // Lock mutex
    app->mutex.lock();

    if (!app->messages.empty()) {

        // Get the first message on the queue
        message = app->messages.front();

        // Pop the message off of said queue
        app->messages.pop();
    }

    // Unlock mutex
    app->mutex.unlock();

    return message;
}

/**
 *
 * Registers this object on the current thread for event processing
 *
 * @param object The object to register
 * @return void
 */
void Application::registerObject(SmartObject *object)
{
    auto app = Application::getInstance();
    auto objects = app->objects.get();

    if (std::find(objects->begin(), objects->end(), object) == objects->end()) {

        // Add to collection
        objects->push_back(object);
    }
}

/**
 *
 * Unregisters this object on the current thread
 *
 * @param object The object to register
 * @return void
 */
void Application::unregisterObject(SmartObject *object)
{
    auto app = Application::getInstance();
    auto objects = app->objects.get();
    auto it = std::find(objects->begin(), objects->end(), object);

    if (it != objects->end()) {

        // Remove from collection
        objects->erase(it);
    }
}

/**
 *
 * Performs shutdown tasks (deletes any remaining objects)
 *
 * @return void
 */
Application::~Application()
{
    auto objects = this->objects.get();

    for (auto it = objects->begin(); it != objects->end(); ++it) {

        // Ask the object to not bother disconnecting from the application. This
        // is to reduce shutdown time if there are a lot of objects
        (*it)->disconnectFromApplication = false;

        // Free memory
        delete *it;
    }
    Application::instance = nullptr;
}

} // namespace RabidSQL


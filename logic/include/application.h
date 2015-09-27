#ifndef RABIDSQL_APPLICATION_H
#define RABIDSQL_APPLICATION_H

#include <queue>
#include <vector>

#include "structs.h"
#include "threadlocal.h"

namespace RabidSQL {

class SmartObject;
class Application
{
    friend class SmartObject;
public:
    static void postMessage(MessageType type, std::string label, Variant data);
    static void processEvents();
    static Message getNextMessage();
    static void shutdown();

private:
    Application() {}

    static Application *instance;
    static Application *getInstance();

    static void registerObject(SmartObject *object);
    static void unregisterObject(SmartObject *object);

    ThreadLocal<std::vector<SmartObject *> > objects;
    std::queue<Message> messages;
    std::mutex mutex;
};

} // namespace RabidSQL

#endif // RABIDSQL_APPLICATION_H

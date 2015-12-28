#ifndef RABIDSQL_APPLICATION_H
#define RABIDSQL_APPLICATION_H

#include <queue>
#include <vector>

#include "ThreadLocal.h"
#include "Message.h"

namespace RabidSQL {

class SmartObject;
class Application
{
    friend class SmartObject;
public:
    static Application *getInstance();

    void postMessage(MessageType type, std::string label, Variant data);
    void processEvents();
    bool hasMessage();
    Message getNextMessage();
    ~Application();

private:
    Application();

    static Application *instance;

    void registerObject(SmartObject *object);
    void unregisterObject(SmartObject *object);

    ThreadLocal<std::vector<SmartObject *> > objects;
    std::queue<Message> messages;
    std::mutex mutex;
};

} // namespace RabidSQL

#endif // RABIDSQL_APPLICATION_H

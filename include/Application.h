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

    virtual void postMessage(MessageType type, std::string label, Variant data);
    virtual void processEvents();
    virtual bool hasMessage();
    virtual Message getNextMessage();
    ~Application();

protected:
    Application();
    virtual void registerObject(SmartObject *object);
    virtual void unregisterObject(SmartObject *object);

private:
    static Application *instance;
    ThreadLocal<std::vector<SmartObject *> > objects;
    std::queue<Message> messages;
    std::mutex mutex;
};

} // namespace RabidSQL

#endif // RABIDSQL_APPLICATION_H

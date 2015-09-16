#ifndef RABIDSQL_RSQLOBJECT_H
#define RABIDSQL_RSQLOBJECT_H

#include <map>
#include <queue>
#include <vector>
#include <string>
#include <typeinfo>

#include "variant.h"
#include "structs.h"

namespace RabidSQL {

class Application;
class SmartObject
{
    friend class Application;
public:
    SmartObject(SmartObject *parent = nullptr);
    virtual SmartObject *getParent();
    virtual std::vector<SmartObject *> getChildren();
    virtual void setParent(SmartObject *parent = nullptr);
    virtual ~SmartObject();
    static std::string tr(std::string text);
    void connectQueue(int id, SmartObject *receiver);
    void disconnectQueue(int id = 0);
    void disconnectQueue(const SmartObject *receiver = nullptr, int id = 0);

protected:
    void queueData(int id, const VariantList &arguments);
    virtual void processQueueItem(int id, VariantList arguments);

private:
    void processQueue();
    void addChild(SmartObject *child);
    void removeChild(SmartObject *child, bool deallocate = true);

    class Data {
    public:
        Data(int id, const VariantList &arguments);
        int id;
        VariantList arguments;
    };

    SmartObject *parent;
    std::vector<SmartObject *> children;
    std::multimap<int, SmartObject *> connectedObjects;
    std::queue<Data> dataQueue;
    std::mutex mutex;

    bool disconnectFromParent;
    bool disconnectFromApplication;
};

} // namespace RabidSQL

#endif // RABIDSQL_RSQLOBJECT_H

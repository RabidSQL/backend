#ifndef RABIDSQL_RSQLOBJECT_H
#define RABIDSQL_RSQLOBJECT_H

#include <map>
#include <mutex>
#include <queue>
#include <vector>
#include <string>
#include <typeinfo>

#include "Variant.h"

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
    void processQueue();
    void setArbitraryData(std::string key, const Variant &value);
    Variant getArbitraryData(std::string key);

protected:
    void queueData(int id, const VariantVector &arguments);
    virtual void processQueueItem(const int id, const VariantVector &arguments);

private:
    void addChild(SmartObject *child);
    void removeChild(SmartObject *child, bool deallocate = true);

    class Data {
    public:
        Data(int id, const VariantVector &arguments);
        int id;
        VariantVector arguments;
    };

    SmartObject *parent;
    std::vector<SmartObject *> children;
    std::multimap<int, SmartObject *> connectedObjects;
    std::map<std::string, Variant> arbitraryData;
    std::queue<Data> dataQueue;
    std::mutex mutex;

    bool disconnectFromParent;
    bool disconnectFromApplication;
};

} // namespace RabidSQL

#endif // RABIDSQL_RSQLOBJECT_H

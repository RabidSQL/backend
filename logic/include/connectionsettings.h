#ifndef RABIDSQL_CONNECTIONSETTINGS_H
#define RABIDSQL_CONNECTIONSETTINGS_H

#include <string>
#include <map>

#include "structs.h"
#include "smartobject.h"
#include "variant.h"

namespace RabidSQL {

#include "enums.h"

class JsonStream;
class ConnectionSettings : public SmartObject
{
public:
    explicit ConnectionSettings(ConnectionSettings *parent = 0);
    ConnectionSettings *getParent();
    virtual void setParent(ConnectionSettings *parent);
    void remove(std::string key);
    bool contains(std::string key);
    Variant get(std::string key, bool bubble = true);
    void set(std::string key, const Variant value);
    ConnectionType getType();

    static std::vector<ConnectionSettings *> load(FileFormat format,
        std::string filename);
    static std::vector<ConnectionSettings *> loadBinary(std::string filename);
    static std::vector<ConnectionSettings *> loadJson(std::string filename);

    static void save(std::vector<ConnectionSettings *> &settings,
        FileFormat format, std::string filename);

    void operator<<(const VariantMap &value);
    void operator>>(VariantVector &value);
    ~ConnectionSettings();

private:
    VariantMap toVariantMap();
    VariantVector getChildren(ConnectionSettings *parent);
    static void reparentChildren(
            std::vector<ConnectionSettings *> &connectionList,
            ConnectionSettings *currentSettings);
    typedef std::map<std::string, Variant> Settings;
    Settings settings;
};

} // namespace RabidSQL

#endif // RABIDSQL_CONNECTIONSETTINGS_H

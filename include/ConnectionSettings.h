#ifndef RABIDSQL_CONNECTIONSETTINGS_H
#define RABIDSQL_CONNECTIONSETTINGS_H

#include <string>
#include <map>

#include "ArbitraryPointer.h"
#include "NSEnums.h"
#include "SmartObject.h"
#include "Variant.h"

namespace RabidSQL {

class JsonFileStream;
class ConnectionSettings : public SmartObject, public ArbitraryPointer
{
public:
    explicit ConnectionSettings(ConnectionSettings *parent = 0);
    virtual ConnectionSettings *getParent();
    virtual void setParent(ConnectionSettings *parent);
    virtual void remove(std::string key);
    virtual bool contains(std::string key);
    virtual Variant get(std::string key, bool bubble = true);
    virtual void set(std::string key, const Variant value);
    virtual ConnectionType getType();
    virtual ConnectionSettings *clone();

    static std::vector<ConnectionSettings *> load(FileFormat format,
        std::string filename);

    static void save(std::vector<ConnectionSettings *> &settings,
        FileFormat format, std::string filename);

    virtual void operator<<(const VariantMap &value);
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

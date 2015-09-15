#ifndef RABIDSQL_CONNECTIONSETTINGS_H
#define RABIDSQL_CONNECTIONSETTINGS_H

#include <string>
#include <map>

#include "structs.h"
#include "smartobject.h"
#include "variant.h"

namespace RabidSQL {

class ConnectionSettings : public SmartObject
{
public:
    typedef enum { INHERIT, MYSQL } type;

    explicit ConnectionSettings(ConnectionSettings *parent = 0);
    ConnectionSettings *getParent();
    virtual void setParent(ConnectionSettings *parent);
    void remove(std::string key);
    bool contains(std::string key);
    Variant get(std::string key, bool bubble = true);
    void set(std::string key, const Variant value);
    type getType();

    static std::vector<ConnectionSettings *> load(FileFormat::format format,
        std::string filename);
    static std::vector<ConnectionSettings *> loadBinary(std::string filename);
    static std::vector<ConnectionSettings *> loadJson(std::string filename);

    static void save(std::vector<ConnectionSettings *> &settings,
        FileFormat::format format, std::string filename, bool destroy);
    static void saveBinary(std::vector<ConnectionSettings *> &settings,
                           std::string filename);
    static void saveJson(std::vector<ConnectionSettings *> &settings,
                         std::string filename);

    void operator<<(const VariantList &value);
    void operator>>(VariantList &value);
    ~ConnectionSettings();

private:
    typedef std::map<std::string, Variant> Settings;
    Settings settings;
};

} // namespace RabidSQL

#endif // RABIDSQL_CONNECTIONSETTINGS_H

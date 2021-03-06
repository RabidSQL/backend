#ifndef RABIDSQL_VARIANT_H
#define RABIDSQL_VARIANT_H

#include "NSEnums.h"

#include <map>
#include <string>
#include <vector>

namespace RabidSQL {

class ArbitraryPointer;
struct QueryResult;
class BinaryFileStream;
class JsonFileStream;
class VariantVector;
class VariantMap;
class Variant
{
    friend class FileStream;
    friend class BinaryFileStream;
    friend class JsonFileStream;
public:
    Variant();
    Variant(const std::nullptr_t &value);
    Variant(const Variant &value);
    Variant(const std::string &value);
    Variant(const char *value);
    Variant(ArbitraryPointer *value, bool manage);
    Variant(const std::vector<std::string> &value);
    Variant(const VariantVector &value);
    Variant(const VariantMap &value);
    Variant(const long &value);
    Variant(const long long &value);
    Variant(const unsigned long &value);
    Variant(const unsigned long long &value);
    Variant(const int &value);
    Variant(const unsigned int &value);
    Variant(const short &value);
    Variant(const unsigned short &value);
    Variant(const bool &value);
    Variant(const double &value);
    Variant(const float &value);
    Variant(const QueryResult &value);
    ~Variant();
    ArbitraryPointer *toPointer() const;
    const std::string toString() const;
    const std::vector<std::string> toStringVector() const;
    const VariantVector toVariantVector() const;
    const VariantMap toVariantMap() const;
    const long toLong() const;
    const long long toLongLong() const;
    const unsigned long toULong() const;
    const unsigned long long toULongLong() const;
    const int toInt() const;
    const unsigned int toUInt() const;
    const short toShort() const;
    const unsigned short toUShort() const;
    const bool toBool() const;
    const float toFloat() const;
    const double toDouble() const;
    const bool isNull() const;
    const QueryResult toQueryResult() const;
    void operator=(const Variant &value);
    bool operator!=(const Variant &value) const;
    bool operator==(const Variant &value) const;
    bool operator>(const Variant &value) const;
    bool operator<(const Variant &value) const;
    bool operator>=(const Variant &value) const;
    bool operator<=(const Variant &value) const;
    const DataType getType() const;

private:
    template<typename T>
    T numericCast() const;
    void init(DataType type);
    void deinit();

    void *data;
    bool deleteData;
    DataType type;
};

class VariantVector : public std::vector<Variant> {

public:
    VariantVector &operator<<(const Variant &value);
    const Variant toVariant();
};

class VariantMap : public std::map<std::string, Variant> {

public:
};

} // namespace RabidSQL

#endif // RABIDSQL_VARIANT_H

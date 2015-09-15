#ifndef RABIDSQL_VARIANT_H
#define RABIDSQL_VARIANT_H

#include <string>
#include <sstream>
#include <vector>

namespace RabidSQL {

struct QueryResult;
class BinaryStream;
class JsonStream;
class VariantList;
class Variant
{
public:
    Variant();
    Variant(const std::nullptr_t &value);
    Variant(const Variant &value);
    Variant(const std::string &value);
    Variant(const char *value);
    Variant(const std::vector<std::string> &value);
    Variant(const VariantList &value);
    Variant(const long &value);
    Variant(const unsigned long &value);
    Variant(const int &value);
    Variant(const unsigned int &value);
    Variant(const short &value);
    Variant(const unsigned short &value);
    Variant(const double &value);
    Variant(const float &value);
    Variant(const QueryResult &value);
    ~Variant();
    const std::string toString() const;
    const std::vector<std::string> toStringVector() const;
    const VariantList toVariantVector() const;
    const long toLong() const;
    const unsigned long toULong() const;
    const int toInt() const;
    const unsigned int toUInt() const;
    const short toShort() const;
    const unsigned short toUShort() const;
    const float toFloat() const;
    const double toDouble() const;
    const bool isNull() const;
    const QueryResult &toQueryResult() const;
    void operator=(const Variant &value);
    bool operator!=(const Variant &value) const;
    bool operator==(const Variant &value) const;
    bool operator>(const Variant &value) const;
    bool operator<(const Variant &value) const;
    bool operator>=(const Variant &value) const;
    bool operator<=(const Variant &value) const;
    void operator<<(JsonStream &value);
    void operator<<(BinaryStream &value);
    void operator>>(JsonStream &value);
    void operator>>(BinaryStream &value);

private:
    std::string readString(BinaryStream &stream);

    template<typename T>
    T numericCast() const;

    enum DataType {
        NONE,
        STRING,
        STRINGVECTOR,
        VARIANTVECTOR,
        ULONG,
        LONG,
        UINT,
        INT,
        USHORT,
        SHORT,
        QUERYRESULT,
        FLOAT,
        DOUBLE,
        _FIRST = NONE,
        _LAST = DOUBLE
    };

    void *data;
    DataType type;
};

class VariantList: public std::vector<Variant> {

public:
    VariantList &operator<<(const Variant &value);
    void operator<<(JsonStream &value);
    void operator<<(BinaryStream &value);
    void operator>>(JsonStream &value);
    void operator>>(BinaryStream &value);
    const Variant toVariant();
};

} // namespace RabidSQL

#endif // RABIDSQL_VARIANT_H

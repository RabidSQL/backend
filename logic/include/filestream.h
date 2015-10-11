#ifndef RABIDSQL_FILESTREAM_H
#define RABIDSQL_FILESTREAM_H

#include "structs.h"

#include <fstream>
#include <stack>

namespace rapidjson
{
    typedef unsigned SizeType;
} // rapidjson

namespace RabidSQL
{

class FileStream : public std::fstream {
public:
    virtual bool open(std::string filename, std::ios_base::openmode mode);
    virtual FileFormat getFormat() = 0;
    virtual FileStream &operator<<(const Variant &value) = 0;
    virtual FileStream &operator>>(Variant &value) = 0;
};

struct JsonHandler {
public:
    enum Type {
        PRIMITIVE, ARRAY, OBJECT
    };
    class Data {
    public:
        Data();
        Data(const Variant &primitive);
        Data(const VariantVector &list);
        Data(const VariantMap &object);
        Type type();
        Variant *primitive();
        VariantVector *array();
        VariantMap *object();
        ~Data();

    private:
        void switchType(Type type);

        Type _type;
        void *_data;
    };

    std::stack<Data *> stack;
    std::stack<std::string> keys;

    bool set(Variant data);
    Variant get();
    bool Null();
    bool Bool(bool b);
    bool Int(int i);
    bool Uint(unsigned u);
    bool Int64(int64_t i);
    bool Uint64(uint64_t u);
    bool Double(double d);
    bool String(const char* str, rapidjson::SizeType length, bool copy);
    bool StartObject();
    bool Key(const char* str, rapidjson::SizeType length, bool copy);
    bool EndObject(rapidjson::SizeType memberCount);
    bool StartArray();
    bool EndArray(rapidjson::SizeType elementCount);
};

class JsonStream : virtual public FileStream {
public:
    typedef char Ch;
    void *writer;

    FileFormat getFormat();
    std::string prepare(std::string string);
    JsonStream();
    virtual ~JsonStream();
    FileStream &operator<<(const Variant &value);
    FileStream &operator>>(Variant &value);

    // Writer methods
    void Put(Ch);
    void Flush();
    Ch* PutBegin();
    size_t PutEnd(Ch*);

    // Reader methods
    Ch Peek() const;
    Ch Take();
    size_t Tell() const;

private:
    char current;
    unsigned int count;
};

class BinaryStream : virtual public FileStream {
public:
    bool open(std::string filename, std::ios_base::openmode mode);
    void mark();
    bool expectMark();
    FileFormat getFormat();
    virtual ~BinaryStream() {}
    FileStream &operator<<(const Variant &value);
    FileStream &operator>>(Variant &value);
private:
    std::string readString();
};

} // namespace RabidSQL

#endif //RABIDSQL_FILESTREAM_H

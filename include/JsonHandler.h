#ifndef RABIDSQL_JSONHANDLER_H
#define RABIDSQL_JSONHANDLER_H

#include "Variant.h"

#include <stack>

namespace RabidSQL {

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
    bool Int64(long long int i);
    bool Uint64(unsigned long long int u);
    bool Double(double d);
    bool String(const char* str, unsigned int length, bool copy);
    bool StartObject();
    bool Key(const char* str, unsigned int length, bool copy);
    bool EndObject(unsigned int memberCount);
    bool StartArray();
    bool EndArray(unsigned int elementCount);
};

} // namesapce RabidSQL

#endif //RABIDSQL_JSONHANDLER_H

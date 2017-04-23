#include "JsonHandler.h"

namespace RabidSQL {

/**
 * Sets or appends the top element in the stack with the value provided
 *
 * @return bool True on success or false on failure
 */
bool JsonHandler::set(Variant data) {

    if (stack.empty()) {

        stack.push(new Data(data));

        return true;
    }

    auto top = stack.top();

    switch (top->type()) {
        case PRIMITIVE:

            // Root is a primitive
            *stack.top()->primitive() = data;
            break;
        case ARRAY:
            stack.top()->array()->push_back(data);
            break;
        case OBJECT:
            if (keys.empty()) {

                // Empty key
                return false;
            }

            (*top->object())[keys.top()] = data;
            keys.pop();
            break;
    }

    return true;
}

/**
 * Gets a variant representing the JSON data. This deletes all of the local data
 * at the same time.
 *
 * @return Variant
 */
Variant JsonHandler::get()
{
    Variant value;
    auto root = stack.top();

    switch (root->type()) {
        case JsonHandler::PRIMITIVE:
            value = *root->primitive();
            break;
        case JsonHandler::ARRAY:
            value = *root->array();
            break;
        case JsonHandler::OBJECT:
            value = *root->object();
            break;
    }

    while (!stack.empty()) {

        delete stack.top();
        stack.pop();
    }

    return value;
}

/**
 * Sets the current value to Null
 *
 * @return bool True on success, false on failure
 */
bool JsonHandler::Null()
{
    return set(nullptr);
}

/**
 * Sets the current value to the provided boolean value
 *
 * @param bool
 * @return bool True on success, false on failure
 */
bool JsonHandler::Bool(bool b)
{
    return set(b);
}

/**
 * Sets the current value to the provided numeric value
 *
 * @param i
 * @return bool True on success, false on failure
 */
bool JsonHandler::Int(int i)
{
    return set(i);
}

/**
 * Sets the current value to the provided unsigned numeric value
 *
 * @param u
 * @return bool True on success, false on failure
 */
bool JsonHandler::Uint(unsigned u)
{
    return set(u);
}

/**
 * Sets the current value to the provided 64bit int value
 *
 * @param i
 * @return bool True on success, false on failure
 */
bool JsonHandler::Int64(long long int i)
{
    return set(i);
}

/**
 * Sets the current value to the provided 64bit (unsigned) int value
 *
 * @param u
 * @return bool True on success, false on failure
 */
bool JsonHandler::Uint64(unsigned long long int u)
{
    return set(u);
}

/**
 * Sets the current value to the provided double value
 *
 * @param d
 * @return bool True on success, false on failure
 */
bool JsonHandler::Double(double d)
{
    return set(d);
}

/**
 * Set the current value to a string-representation of a number.
 * @param str
 * @param length
 * @param copy
 * @return bool
 */
bool JsonHandler::RawNumber(const char* str, unsigned int length, bool copy)
{
    // Note that Variant automatically converts internally as needed so lets not bother converting ourselves.
    return set(str);
}

/**
 * Sets the current value to the provided string value. This must be UTF8.
 *
 * @param str
 * @param length
 * @param copy
 * @return bool True on success, false on failure
 */
bool JsonHandler::String(const char* str, unsigned int length, bool copy)
{
    return set(str);
}

/**
 * Starts a variant map on top of the stack
 *
 * @return bool True on success, false on failure
 */
bool JsonHandler::StartObject()
{
    stack.push(new Data(VariantMap()));

    return true;
}

/**
 * Pushes a key onto the key stack for use in processing objects. This must be
 * UTF8
 *
 * @param str
 * @param length
 * @pram copy
 * @return bool True on success, false on failure
 */
bool JsonHandler::Key(const char* str, unsigned int length, bool copy)
{
    keys.push(str);
    return true;
}

/**
 * Closes an object. Returns false if the data is bad.
 *
 * @param memberCount
 * @return bool True on success, false on failure
 */
bool JsonHandler::EndObject(unsigned int memberCount)
{
    auto size = stack.size();

    if (size == 0) {

        // Invalid data
        return false;
    } else if (size == 1) {

        // This is the root node. We don't need to do anything else
        return true;
    }

    std::unique_ptr<Data> data(stack.top());
    stack.pop();

    if (data->type() != OBJECT) {

        // Invalid data
        return false;
    }

    auto object = data->object();

    if (object->size() != memberCount) {

        // Invalid data
        return false;
    }

    auto top = stack.top();

    switch (top->type()) {
        case PRIMITIVE:

            // This is an impossible case. Mark as a parsing error
            return false;
        case ARRAY:
            top->array()->push_back(*object);
            break;
        case OBJECT:
            if (keys.empty()) {

                // Empty key
                return false;
            }
            (*top->object())[keys.top()] = *object;
            keys.pop();
            break;
    }

    return true;
}

/**
 * Pushes a new array onto the top of the stack
 *
 * @return bool True on success, false on failure
 */
bool JsonHandler::StartArray()
{
    stack.push(new Data(VariantVector()));

    return true;
}

/**
 * Closes an array. Returns false if the data is bad.
 *
 * @param elementCount
 * @return bool True on success, false on failure
 */
bool JsonHandler::EndArray(unsigned int elementCount)
{
    auto size = stack.size();

    if (size == 0) {

        // Invalid data
        return false;
    } else if (size == 1) {

        // This is the root node. We don't need to do anything else
        return true;
    }

    std::unique_ptr<Data> data(stack.top());
    stack.pop();

    if (data->type() != ARRAY) {

        // Invalid data
        return false;
    }

    auto array = data->array();

    if (array->size() != elementCount) {

        // Invalid data
        return false;
    }

    auto top = stack.top();

    switch (top->type()) {
        case PRIMITIVE:

            // This is an impossible case. Mark as a parsing error
            return false;
        case ARRAY:
            top->array()->push_back(*array);
            break;
        case OBJECT:
            if (keys.empty()) {

                // Empty key
                return false;
            }
            (*top->object())[keys.top()] = *array;
            keys.pop();
            break;
    }

    return true;
}

/**
 * Initializes an empty Data instance.
 *
 * @return void
 */
JsonHandler::Data::Data()
{
    _data = nullptr;
}

/**
 * Initializes a Data instance using the data from the provided primitive.
 * Note that while Variant does support both VariantVector and VariantMap,
 * neither is supported in this constructor.
 *
 * @return void
 */
JsonHandler::Data::Data(const Variant &primitive)
{
    _data = nullptr;
    *this->primitive() = primitive;
}

/**
 * Initializes a Data instance using the data from the provided vector.
 *
 * @return void
 */
JsonHandler::Data::Data(const VariantVector &list)
{
    _data = nullptr;
    *this->array() = list;
}

/**
 * Initializes a Data instance using the data from the provided object.
 *
 * @return void
 */
JsonHandler::Data::Data(const VariantMap &object)
{
    _data = nullptr;
    *this->object() = object;
}

/**
 * Switches to the provided type. Frees memory if applicable
 *
 * @param type
 * @return void
 */
void JsonHandler::Data::switchType(JsonHandler::Type type)
{

    if (_data != nullptr) {

        if (_type == type) {

            // We're already using the correct type
            return;
        }

        // Free memory for whatever was allocated previously
        delete reinterpret_cast<char *>(_data);
        _data = nullptr;
    }

    _type = type;
    switch (type) {
        case PRIMITIVE:
            _data = new Variant();
            break;
        case ARRAY:
            _data = new VariantVector();
            break;
        case OBJECT:
            _data = new VariantMap();
            break;
    }
}

/**
 * Returns the current type
 *
 * @return Type
 */
JsonHandler::Type JsonHandler::Data::type()
{
    return _type;
}

/**
 * Returns the current primitive. If this is not currently a primitive, it is
 * deleted and a blank primitive returned.
 *
 * @return Variant
 */
Variant *JsonHandler::Data::primitive()
{
    switchType(PRIMITIVE);

    return static_cast<Variant *>(_data);
}

/**
 * Returns the current array. If this is not currently a array, it is
 * deleted and a blank array returned.
 *
 * @return VariantVector
 */
VariantVector *JsonHandler::Data::array()
{
    switchType(ARRAY);

    return static_cast<VariantVector *>(_data);
}

/**
 * Returns the current object. If this is not currently a array, it is
 * deleted and a blank object returned.
 *
 * @return VariantMap
 */
VariantMap *JsonHandler::Data::object()
{
    switchType(OBJECT);

    return static_cast<VariantMap *>(_data);
}

/**
 * Destroys the current data
 *
 * @return void
 */
JsonHandler::Data::~Data()
{
    if (_data != nullptr) {

        // Free memory for whatever was allocated previously
        delete reinterpret_cast<char *>(_data);
    }
}

} // namespace RabidSQL
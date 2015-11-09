#include "app.h"
#include "filestream.h"
#include "libs/rapidjson/include/rapidjson/reader.h"
#include "libs/rapidjson/include/rapidjson/error/en.h"
#include "libs/rapidjson/include/rapidjson/writer.h"
#include "libs/rapidjson/include/rapidjson/stringbuffer.h"
#include <memory>

namespace RabidSQL {

/**
 *
 * Opens the file specified in the mode specified and returns a boolean
 * indicating whether it opened successfully or not
 *
 * @param filename The filename to open
 * @param mode The mode to open the file in
 * @return bool
 */
bool FileStream::open(std::string filename, std::ios_base::openmode mode)
{
    std::fstream::open(filename, mode);

    return is_open();
}

/**
 *
 * Opens the file specified in the mode specified ( + binary) and returns a
 * boolean indicating whether it opened successfully or not. This advanced the
 * stream up to 6 bytes.
 *
 * @param filename The filename to open
 * @param mode The mode to open the file in
 * @return bool
 */
bool BinaryStream::open(std::string filename, std::ios_base::openmode mode)
{
    // Remove seek to end flag. We need to verify the header and that would add
    // an extra seek
    std::fstream::open(filename, std::ios::binary | (mode & ~std::ios::ate));

    if (is_open()) {

        if (mode & std::ios::in) {

            char format[7];

            // Check format
            read(format, 6);

            // Ensure null-termination
            format[6] = 0;

            auto fileFormat = std::string(format);
            if (fileFormat != "RSQAF0") {

                if (mode & std::ios::out) {

                    // We're in read-write mode. If the header is wrong, the
                    // file may be corrupt. If we got any data then we'll return
                    // this as invalid. If there was no data, then this is
                    // just a new file
                    if (fileFormat.size() > 0) {

                        // Close file
                        close();

                        // Return that the open was unsuccessful
                        return false;
                    }
                } else {

                    // Close file
                    close();

                    // Return that the open was unsuccessful
                    return false;
                }
            }
        }

        if (mode & std::ios::out) {

            // Write file header
            write("RSQAF0", 6);
        }

        if (mode & std::ios::ate) {

            // Seek to end of file
            seekg(0, std::ios::end);
        }

        // Success!
        return true;
    }

    // Open failed
    return false;
}

/**
 *
 * Returns the format of this stream. For possible future use.
 *
 * @return FileFormat
 */
FileFormat BinaryStream::getFormat()
{
    return BINARY;
}

/**
 *
 * Expects a marker in the binary file to be present. This advances the stream
 * up to 3 bytes.
 *
 * @return bool
 */
bool BinaryStream::expectMark()
{
    if (!is_open() || eof()) {

        // Can't perform a read
        return false;
    }

    char marker[4];

    // Check for marker
    read(marker, 3);

    // Ensure null-termination
    marker[3] = 0;

    return std::string(marker) == "SOL";
}

/**
 *
 * Writes a marker to the binary file for identifying a new section
 *
 * @return void
 */
void BinaryStream::mark()
{
    write("SOL", 3);
}

/**
 * Reads a string from the open binary stream
 *
 * @return string
 */
std::string BinaryStream::readString()
{
    long size = 0;
    char *data;
    std::string string;

    if (eof()) {
        return "";
    }

    // Get number of characters
    read(reinterpret_cast<char *>(&size), sizeof(long));

    if (eof()) {

        // Set to empty string
        string = "";
    } else {

        // Initialize data. Use an extra byte because we always want to make
        // 100% sure that it is null terminated
        data = new char[size + 1];

        // Read data
        read(data, size);

        // Ensure this is nullptr terminated (prevents issues if there is a
        // corrupt string)
        data[size] = 0;

        // Convert to std string
        string = std::string(data);

        // Free memory.
        delete[] data;
    }

    return string;
}

/**
 *
 * Reads a variant from the open binary stream
 *
 * @param value The value to write to
 * @return FileStream a reference to the current stream
 */
FileStream &BinaryStream::operator>>(Variant &value)
{
    #ifdef DEBUG
    rDebug << "READ";
    #endif
    DataType type;
    long count = 0;
    int data = 0;

    if (eof()) {

        // No more data
        return *this;
    }

    // Get the type. Put it in a long first so we can error-check it
    read(reinterpret_cast<char *>(&data), sizeof(data));

    assert(data >= _FIRST);
    assert(data <= _LAST);

    if (data < _FIRST || data > _LAST) {

        // Invalid data
        return *this;
    }

    // Convert data
    type = static_cast<DataType>(data);

    if (type != D_NULL && eof()) {

        // Invalid data
        return *this;
    }

    switch (type) {
        case D_NULL:
            // Variant is nullptr by default. We need not do any more work
            value = nullptr;
            break;
        case D_STRING:
            value = readString();
            break;
        case D_STRINGVECTOR:
            // Get number of strings
            read(reinterpret_cast<char *>(&count), sizeof(count));
            if (count > 0) {
                std::vector<std::string> vector;

                for (auto i = 0; i < count; i++) {

                    // Read a string and push it into the vector
                    vector.push_back(readString());
                }

                value = vector;
            }
            break;
        case D_VARIANTVECTOR:
            // Get number of variants
            read(reinterpret_cast<char *>(&count), sizeof(count));

            if (count > 0) {
                VariantVector vector;

                for (auto i = 0; i < count; i++) {

                    Variant value;

                    // Read from stream
                    *this >> value;

                    // Read a string and push it into the vector
                    vector.push_back(value);
                }

                value = vector;
            }
            break;
        case D_VARIANTMAP:
            // Get number of elements
            read(reinterpret_cast<char *>(&count), sizeof(count));

            if (count > 0) {
                VariantMap map;

                for (auto i = 0; i < count; i++) {

                    Variant key;
                    Variant value;

                    // Read from stream
                    *this >> key;
                    *this >> value;

                    // Read a string and push it into the vector
                    map[key.toString()] = value;
                }

                value = map;
            }
            break;
        case D_LONG:
        {
            long data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_LONGLONG:
        {
            long long data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_ULONG:
        {
            unsigned long data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_ULONGLONG:
        {
            unsigned long long data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_INT:
        {
            int data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_UINT:
        {
            unsigned int data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_SHORT:
        {
            short data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_USHORT:
        {
            unsigned short data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_BOOLEAN:
        {
            bool data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_FLOAT:
        {
            float data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_DOUBLE:
        {
            double data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case D_QUERYRESULT:
            #ifdef DEBUG
            rDebug << "QueryResult binary loading not implemented!";
            #endif

            value = QueryResult();
            break;
    }

    return *this;
}

/**
 *
 * Writes a variant to the open binary stream
 *
 * @param value The value to read from
 * @return FileStream a reference to the current stream
 */
FileStream &BinaryStream::operator<<(const Variant &value)
{
    // Write the data type
    int type = value.type;
    write((char *)(&type), sizeof(type));

    switch (type) {
        case D_NULL:
            // Variant is nullptr by default. We need not do any more work
            break;
        case D_STRING:
        {
            std::string data = value.toString();
            auto size = data.size();
            write(reinterpret_cast<char *>(&size), sizeof(size));
            write(data.c_str(), size);
            break;
        }
        case D_STRINGVECTOR:
        {
            auto vector(value.toStringVector());
            auto count = vector.size();

            write(reinterpret_cast<char *>(&count), sizeof(count));
            if (count > 0) {
                for (auto it = vector.begin(); it != vector.end(); ++it) {
                    auto data = *it;
                    auto size = data.size();
                    write(reinterpret_cast<char *>(&size), sizeof(size));
                    write(data.c_str(), size);
                }
            }
            break;
        }
        case D_VARIANTVECTOR:
        {
            auto vector(value.toVariantVector());
            auto count = vector.size();

            write(reinterpret_cast<char *>(&count), sizeof(count));
            if (count > 0) {
                for (auto it = vector.begin(); it != vector.end(); ++it) {
                    Variant data = *it;
                    *this << data;
                }
            }
            break;
        }
        case D_VARIANTMAP:
        {
            auto map(value.toVariantMap());
            auto count = map.size();

            write(reinterpret_cast<char *>(&count), sizeof(count));
            if (count > 0) {
                for (auto it = map.cbegin(); it != map.cend(); ++it) {
                    *this << Variant(it->first);
                    *this << it->second;
                }
            }
            break;
        }
        case D_QUERYRESULT:
            #ifdef DEBUG
            rDebug << "Attempting to write QueryResult to stream. WARNING: This"
                << "will write/return only an empty QueryResult structure!";
            #endif
            break;
        case D_LONG:
        {
            auto data = value.toLong();
            write((char *)(&data), sizeof(data));
            break;
        }
        case D_ULONG:
        {
            auto data = value.toULong();
            write((char *)(&data), sizeof(data));
            break;
        }
        case D_INT:
        {
            auto data = value.toInt();
            write((char *)(&data), sizeof(data));
            break;
        }
        case D_UINT:
        {
            auto data = value.toUInt();
            write((char *)(&data), sizeof(data));
            break;
        }
        case D_SHORT:
        {
            auto data = value.toShort();
            write((char *)(&data), sizeof(data));
            break;
        }
        case D_USHORT:
        {
            auto data = value.toUShort();
            write((char *)(&data), sizeof(data));
            break;
        }
        case D_BOOLEAN:
        {
            auto data = value.toBool();
            write((char *)(&data), sizeof(data));
            break;
        }
        case D_FLOAT:
        {
            auto data = value.toFloat();
            write((char *)(&data), sizeof(data));
            break;
        }
        case D_DOUBLE:
        {
            auto data = value.toDouble();
            write((char *)(&data), sizeof(data));
            break;
        }
    }

    return *this;
}

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
bool JsonHandler::Int64(int64_t i)
{
    return set(i);
}

/**
 * Sets the current value to the provided 64bit (unsigned) int value
 *
 * @param u
 * @return bool True on success, false on failure
 */
bool JsonHandler::Uint64(uint64_t u)
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
 * Sets the current value to the provided string value. This must be UTF8.
 *
 * @param str
 * @param length
 * @param copy
 * @return bool True on success, false on failure
 */
bool JsonHandler::String(const char* str, rapidjson::SizeType length, bool copy)
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
bool JsonHandler::Key(const char* str, rapidjson::SizeType length, bool copy)
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
bool JsonHandler::EndObject(rapidjson::SizeType memberCount)
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
bool JsonHandler::EndArray(rapidjson::SizeType elementCount)
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

/**
 * Initializes the json stream, setting the writer to a null pointer
 *
 * @return void
 */
JsonStream::JsonStream()
{
    writer = nullptr;
}

/**
 *
 * Returns the format of this stream. For possible future use.
 *
 * @return FileFormat
 */
FileFormat JsonStream::getFormat()
{
    return JSON;
}

/**
 *
 * Writes a variant to the open json stream
 *
 * @param value The value to read from
 * @return FileStream a reference to the current stream
 */
FileStream &JsonStream::operator<<(const Variant &value)
{
    if (writer == nullptr) {

        // Initialize writer
        writer = new rapidjson::Writer<JsonStream>(*this);
    }

    auto writer = reinterpret_cast<rapidjson::Writer<JsonStream> *>(
            this->writer);

    switch (value.type) {
        case D_STRING:
        {
            writer->String(value.toString().c_str());
            break;
        }
        case D_STRINGVECTOR:
        case D_VARIANTVECTOR:
        {
            writer->StartArray();

            // Use variant vector for both string and variant since they will
            // be stored the same
            auto data = value.toVariantVector();

            size_t count = 0;

            for (auto it = data.begin(); it != data.end(); ++it) {

                // Write element
                *this << *it;
                count++;
            }

            writer->EndArray(count);

            break;
        }
        case D_VARIANTMAP:
        {

            writer->StartObject();

            auto map(value.toVariantMap());

            size_t count = 0;

            for (auto it = map.cbegin(); it != map.cend(); ++it) {

                // Write key. Convert to string
                writer->String(it->first.c_str());

                // Write value
                *this << it->second;
                count++;
            }

            writer->EndObject(count);

            break;
        }
        case D_DOUBLE:
        case D_FLOAT:
        case D_SHORT:
        case D_USHORT:
        case D_INT:
        case D_UINT:
        case D_LONG:
        case D_LONGLONG:
        case D_ULONG:
        case D_ULONGLONG:
            writer->Int64(value.toLongLong());
            break;
        case D_BOOLEAN:
            writer->Bool(value.toBool());
            break;
        case D_NULL:
        case D_QUERYRESULT:
            writer->Null();
            break;
    }

    return *this;
}

/**
 *
 * Reads a variant from the open json stream
 *
 * @param value The value to write to
 * @return FileStream a reference to the current stream
 */
FileStream &JsonStream::operator>>(Variant &value)
{
    rapidjson::Reader reader;
    JsonHandler handler;
    current = 0;
    count = 0;
    Take();
    reader.Parse(*this, handler);

    if (reader.HasParseError()) {

        return *this;
    }

    value = handler.get();

    return *this;
}

JsonStream::Ch JsonStream::Peek() const
{
    return current;
}

JsonStream::Ch JsonStream::Take()
{
    Ch c = current;
    current = 0;
    read(reinterpret_cast<char *>(&current), sizeof(current));
    count++;
    return c;
}

size_t JsonStream::Tell() const
{
    if (current != 0) {
        return count + 1;
    }
    return count;
    //return count_ + static_cast<size_t>(current_ - buffer_);
}

void JsonStream::Put(JsonStream::Ch ch)
{
    write(&ch, 1);
}

void JsonStream::Flush()
{
    flush();
}

JsonStream::Ch* JsonStream::PutBegin()
{
    RAPIDJSON_ASSERT(false);
    return 0;
}

size_t JsonStream::PutEnd(JsonStream::Ch*)
{
    RAPIDJSON_ASSERT(false);
    return 0;
}

std::string JsonStream::prepare(std::string string)
{
    std::string buffer("\"");

    for (auto it = string.begin(); it != string.end(); ++it) {
        switch (*it) {
            case '\b':
                buffer.append("\\b");
                break;
            case '\f':
                buffer.append("\\f");
                break;
            case '\n':
                buffer.append("\\n");
                break;
            case '\r':
                buffer.append("\\r");
                break;
            case '\t':
                buffer.append("\\t");
                break;
            case '\"':
                buffer.append("\\\"");
                break;
            case '\\':
                buffer.append("\\\\");
                break;
            default:
                buffer.append(1, *it);
        }
    }

    buffer.append("\"");

    return buffer;
}

JsonStream::~JsonStream()
{
    if (writer != nullptr) {

        delete reinterpret_cast<rapidjson::Writer<JsonStream> *>(this->writer);
    }
}

} // namespace RabidSQL

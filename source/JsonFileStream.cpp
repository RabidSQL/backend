#include "JsonFileStream.h"
#include "JsonHandler.h"
#include "libs/rapidjson/include/rapidjson/reader.h"
#include "libs/rapidjson/include/rapidjson/error/en.h"
#include "libs/rapidjson/include/rapidjson/writer.h"

namespace RabidSQL {


/**
 * Initializes the json stream, setting the writer to a null pointer
 *
 * @return void
 */
JsonFileStream::JsonFileStream()
{
    writer = nullptr;
}

/**
 *
 * Returns the format of this stream. For possible future use.
 *
 * @return FileFormat
 */
RabidSQL::FileFormat JsonFileStream::getFormat()
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
FileStream &JsonFileStream::operator<<(const Variant &value)
{
    if (writer == nullptr) {

        // Initialize writer
        writer = new rapidjson::Writer<JsonFileStream>(*this);
    }

    auto writer = reinterpret_cast<rapidjson::Writer<JsonFileStream> *>(
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

            __darwin_size_t count = 0;

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

            __darwin_size_t count = 0;

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
FileStream &JsonFileStream::operator>>(Variant &value)
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

JsonFileStream::Ch JsonFileStream::Peek() const
{
    return current;
}

JsonFileStream::Ch JsonFileStream::Take()
{
    Ch c = current;
    current = 0;
    std::fstream::read(reinterpret_cast<char *>(&current), sizeof(current));
    count++;
    return c;
}

size_t JsonFileStream::Tell() const
{
    if (current != 0) {
        return count + 1;
    }
    return count;
    //return count_ + static_cast<size_t>(current_ - buffer_);
}

void JsonFileStream::Put(JsonFileStream::Ch ch)
{
    write(&ch, 1);
}

void JsonFileStream::Flush()
{
    flush();
}

JsonFileStream::Ch*JsonFileStream::PutBegin()
{
    RAPIDJSON_ASSERT(false);
    return 0;
}

size_t JsonFileStream::PutEnd(JsonFileStream::Ch*)
{
    RAPIDJSON_ASSERT(false);
    return 0;
}

JsonFileStream::~JsonFileStream()
{
    if (writer != nullptr) {

        delete reinterpret_cast<rapidjson::Writer<JsonFileStream> *>(this->writer);
    }
}

} // namespace RabidSQL
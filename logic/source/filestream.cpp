#include "app.h"
#include "filestream.h"

#include <cstring>

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
FileFormat::format BinaryStream::getFormat()
{
    return FileFormat::BINARY;
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
    Variant::DataType type;
    long count = 0;
    int data = 0;

    if (eof()) {

        // No more data
        return *this;
    }

    // Get the type. Put it in a long first so we can error-check it
    read(reinterpret_cast<char *>(&data), sizeof(data));

    assert(data >= Variant::_FIRST);
    assert(data <= Variant::_LAST);

    if (data < Variant::_FIRST || data > Variant::_LAST) {

        // Invalid data
        return *this;
    }

    // Convert data
    type = static_cast<Variant::DataType>(data);

    if (type != Variant::NONE && eof()) {

        // Invalid data
        return *this;
    }

    switch (type) {
        case Variant::NONE:
            // Variant is nullptr by default. We need not do any more work
            value = nullptr;
            break;
        case Variant::STRING:
            value = readString();
            break;
        case Variant::STRINGVECTOR:
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
        case Variant::VARIANTVECTOR:
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
        case Variant::VARIANTMAP:
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
        case Variant::LONG:
        {
            long data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case Variant::ULONG:
        {
            unsigned long data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case Variant::INT:
        {
            int data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case Variant::UINT:
        {
            unsigned int data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case Variant::SHORT:
        {
            short data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case Variant::USHORT:
        {
            unsigned short data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case Variant::BOOL:
        {
            bool data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case Variant::FLOAT:
        {
            float data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case Variant::DOUBLE:
        {
            double data = 0;
            read(reinterpret_cast<char *>(&data), sizeof(data));

            value = data;
            break;
        }
        case Variant::QUERYRESULT:
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
        case Variant::NONE:
            // Variant is nullptr by default. We need not do any more work
            break;
        case Variant::STRING:
        {
            std::string data = value.toString();
            auto size = data.size();
            write(reinterpret_cast<char *>(&size), sizeof(size));
            write(data.c_str(), size);
            break;
        }
        case Variant::STRINGVECTOR:
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
        case Variant::VARIANTVECTOR:
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
        case Variant::VARIANTMAP:
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
        case Variant::QUERYRESULT:
            #ifdef DEBUG
            rDebug << "Attempting to write QueryResult to stream. WARNING: This"
                << "will write/return only an empty QueryResult structure!";
            #endif
            break;
        case Variant::LONG:
        {
            auto data = value.toLong();
            write((char *)(&data), sizeof(data));
            break;
        }
        case Variant::ULONG:
        {
            auto data = value.toULong();
            write((char *)(&data), sizeof(data));
            break;
        }
        case Variant::INT:
        {
            auto data = value.toInt();
            write((char *)(&data), sizeof(data));
            break;
        }
        case Variant::UINT:
        {
            auto data = value.toUInt();
            write((char *)(&data), sizeof(data));
            break;
        }
        case Variant::SHORT:
        {
            auto data = value.toShort();
            write((char *)(&data), sizeof(data));
            break;
        }
        case Variant::USHORT:
        {
            auto data = value.toUShort();
            write((char *)(&data), sizeof(data));
            break;
        }
        case Variant::BOOL:
        {
            auto data = value.toBool();
            write((char *)(&data), sizeof(data));
            break;
        }
        case Variant::FLOAT:
        {
            auto data = value.toFloat();
            write((char *)(&data), sizeof(data));
            break;
        }
        case Variant::DOUBLE:
        {
            auto data = value.toDouble();
            write((char *)(&data), sizeof(data));
            break;
        }
    }

    return *this;
}

/**
 *
 * Returns the format of this stream. For possible future use.
 *
 * @return FileFormat
 */
FileFormat::format JsonStream::getFormat()
{
    return FileFormat::JSON;
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
    switch (value.type) {
        case Variant::STRING:
        {
            *static_cast<std::fstream *>(this) << prepare(value.toString());
            break;
        }
        case Variant::STRINGVECTOR:
        case Variant::VARIANTVECTOR:
        {
            write("[", 1);

            // Use variant vector for both string and variant since they will
            // be stored the same
            auto data = value.toVariantVector();

            for (auto it = data.begin(); it != data.end(); ++it) {

                if (it != data.begin()) {

                    // Add a comma as there were preceding elements
                    write(",", 1);
                }

                // Write element
                *this << *it;
            }

            write("]", 1);
            break;
        }
        case Variant::VARIANTMAP:
        {
            write("{", 1);

            auto map(value.toVariantMap());

            for (auto it = map.cbegin(); it != map.cend(); ++it) {

                if (it != map.cbegin()) {

                    // Add a comma as there were preceding elements
                    write(",", 1);
                }

                // Write key. Convert to string
                *this << Variant(it->first);

                // Write separator
                write(":", 1);

                // Write value
                *this << it->second;
            }

            write("}", 1);
            break;
        }
        case Variant::DOUBLE:
        case Variant::FLOAT:
        case Variant::SHORT:
        case Variant::USHORT:
        case Variant::INT:
        case Variant::UINT:
        case Variant::LONG:
        case Variant::ULONG:
        {
            *static_cast<std::fstream *>(this) << value.toString();
            break;
        }
        case Variant::BOOL:
            if (value.toBool()) {
                *static_cast<std::fstream *>(this) << "true";
            } else {
                *static_cast<std::fstream *>(this) << "false";
            }
            break;
        case Variant::NONE:
        case Variant::QUERYRESULT:
            *static_cast<std::fstream *>(this) << "null";
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
    char ch = ignoreWhitespace();

    value = readVariant(ch);

    return *this;
}

Variant JsonStream::readNumber()
{
    char ch;
    bool decimal = false;
    std::string buffer;

    while (!eof()) {

        // Read character
        ch = 0;
        read(&ch, 1);

        if (ch == '-') {

            if (buffer.size() > 0) {

                // Not the first character
                // @TODO: raise an error
                return nullptr;
            } else {

                // Add to buffer
                buffer.push_back('-');
            }
        } else if (ch == '.') {

            // Get last character. There is always at least one character
            // because the << operator requires 0-9 or -
            ch = buffer.back();
            decimal = true;

            if (ch >= '0' && ch <= '9') {

                // Add to buffer
                buffer.push_back('.');
            } else {

                // Not a valid number
                // @TODO: raise an error
                return nullptr;
            }
        } else if (ch >= '0' && ch <= '9') {

            // Add to buffer
            buffer.push_back(ch);
        } else {

            // End of the number
            // Rewind 1 byte
            seekp(-1, std::ios_base::cur);

            char lch = tolower(ch);
            if (lch == 'n' || lch == 't' || lch == 'f') {

                // Number is immediately followed by an n, t, or f. This will
                // not be caught by the main stream reader, so we need to report
                // it as an error here
                // @TODO: raise an error
                return nullptr;
            }

            // Valid number and all done processing!
            if (decimal) {

                // Convert to double
                return std::stod(buffer);
            } else {

                // Convert to long
                return std::stol(buffer);
            }
        }
    }

    return nullptr;
}

Variant JsonStream::readString()
{
    char ch;
    std::string buffer;

    while (!eof()) {

        // Read character
        ch = 0;
        read(&ch, 1);

        switch (ch) {
            case '\\':

                // Read next character
                ch = 0;
                read(&ch, 1);

                // Un-escape certain special characters
                switch (ch) {
                    case 'b':
                        buffer.push_back('\b');
                        break;
                    case 'f':
                        buffer.push_back('\f');
                        break;
                    case 'n':
                        buffer.push_back('\n');
                        break;
                    case 'r':
                        buffer.push_back('\r');
                        break;
                    case 't':
                        buffer.push_back('\t');
                        break;
                    case '"':
                        buffer.push_back('"');
                        break;
                    case '\\':
                        buffer.push_back('\\');
                        break;
                    case 'u':
                    {
                        char ch[5] = { 0 };
                        read(ch, 4);

                        // Ensure null termination
                        ch[4] = 0;
                        // decode this!
                    }
                    default:
                        buffer.push_back('\\');
                        buffer.push_back(ch);
                        break;
                }
            case '"':

                // All done!
                return buffer;
            default:

                // Add to buffer;
                buffer.push_back(ch);
        }
    }

    return nullptr;
}

Variant JsonStream::readObject()
{
    VariantMap map;
    char ch;

    while (!eof()) {

        ch = ignoreWhitespace();

        switch (ch) {
            case '}':

                // end of object
                return map;
            case ',':

                // keep processing
                break;
            case '"':
            {

                // Start of a key
                auto key = readString();

                // Get next character
                ch = ignoreWhitespace();

                if (ch != ':') {

                    // Must be a colon (separator), or this is invalid
                    return nullptr;
                }

                // Get next character
                ch = ignoreWhitespace();

                // Read the value and add to our map
                map[key.toString()] = readVariant(ch);
                break;
            }
            default:

                // This is an invalid object
                return nullptr;
        }
    }

    return nullptr;
}

Variant JsonStream::readList()
{
    VariantVector list;
    char ch;

    while (!eof()) {

        ch = ignoreWhitespace();

        switch (ch) {
            case ']':

                // end of list
                return list;
            case ',':

                // keep processing
                continue;
            default:
                list.push_back(readVariant(ch));
        }
    }

    return nullptr;
}

Variant JsonStream::readVariant(char ch)
{
    if (ch >= '0' && ch <= '9') {

        // Set to 0 for easier access in the switch. We'll re-read this byte.
        ch = '0';
    }

    switch (ch) {
        case '"':

            // string
            return readString();
        case '{':

            // object
            return readObject();
        case '[':

            // list
            return readList();
        case 'n':
        case 'N':
        {

            // NULL value
            char buffer[4] = { 0 };
            read(buffer, 3);

            // Ensure null termination
            buffer[3] = 0;

            if (strcasecmp(buffer, "ull") == 0) {

                // Found null!
                return Variant(nullptr);
            }

            // Invalid data
            return nullptr;
        }
        case 't':
        case 'T':
        {

            // TRUE value
            char buffer[4] = { 0 };
            read(buffer, 3);

            // Ensure null termination
            buffer[3] = 0;

            if (strcasecmp(buffer, "rue") == 0) {

                // Found true!
                return Variant(true);
            }

            // Invalid data
            return nullptr;
        }
        case 'f':
        case 'F':
        {

            // FALSE value
            char buffer[5] = { 0 };
            read(buffer, 4);

            // Ensure null termination
            buffer[4] = 0;

            if (strcasecmp(buffer, "alse") == 0) {

                // Found false!
                return Variant(false);
            }

            // Invalid data
            return nullptr;
        }
        case '0':
        case '-':

            // Rewind 1 byte
            seekp(-1, std::ios_base::cur);

            // number
            return readNumber();
        default:

            return nullptr;
    }
}

char JsonStream::ignoreWhitespace()
{
    char ch;

    while (!eof()) {

        // Read byte
        ch = 0;
        read(&ch, 1);

        if (strchr(ws, ch) != nullptr || ch == 0) {

            // Found a whitespace character
            continue;
        }

        return ch;
    }

    // Nothing found
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

char JsonStream::ws[] = { 0x20, 0x09, 0x0a, 0x0d };
} // namespace RabidSQL

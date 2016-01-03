#include "App.h"
#include "BinaryFileStream.h"
#include "QueryResult.h"

namespace RabidSQL {

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
bool BinaryFileStream::open(std::string filename, unsigned int mode)
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
FileFormat BinaryFileStream::getFormat()
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
bool BinaryFileStream::expectMark()
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
void BinaryFileStream::mark()
{
    write("SOL", 3);
}

/**
 * Reads a string from the open binary stream
 *
 * @return string
 */
std::string BinaryFileStream::readString()
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
FileStream &BinaryFileStream::operator>>(Variant &value)
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
        case D_POINTER:
            // We can't store a memory pointer in a file. Set to null.
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
FileStream &BinaryFileStream::operator<<(const Variant &value)
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

} // namespace RabidSQL
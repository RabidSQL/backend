#include "App.h"
#include "FileStream.h"
#include "Variant.h"
#include "QueryResult.h"

#include <cmath>
#include <sstream>

namespace RabidSQL {

/**
 * Basic initialization (sets defaults)
 *
 * @param DataType type The type of Variant we're initializing
 * @return void
 */
void Variant::init(DataType type)
{
    this->type = type;
    this->data = nullptr;
    switch (type) {
        case D_NULL:
        case D_POINTER:
            deleteData = false;
            break;
        case D_STRING:
        case D_STRINGVECTOR:
        case D_VARIANTVECTOR:
        case D_VARIANTMAP:
        case D_LONG:
        case D_LONGLONG:
        case D_ULONG:
        case D_ULONGLONG:
        case D_INT:
        case D_UINT:
        case D_SHORT:
        case D_USHORT:
        case D_BOOLEAN:
        case D_DOUBLE:
        case D_FLOAT:
        case D_QUERYRESULT:
            deleteData = true;
            break;
    }
}

/**
 * Initializes a null variant.
 *
 * @return void
 */
Variant::Variant()
{
    init(D_NULL);
}

/**
 * Initializes a null variant
 *
 * @return void
 */
Variant::Variant(const std::nullptr_t &value)
{
    init(D_NULL);
    // We can technically recover from this being called with a regular pointer,
    // but it is unknown whether we should be managing memory or not. If we do
    // it could lead to crashes and if we do not it could lead to memory leaks...
    // Pointers (except null) should go through Variant(void *...)
    assert(value == nullptr);
}

/**
 *
 * Makes a copy of another variant.
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const Variant &value)
{
    init(D_NULL);
    *this = value;
}

/**
 * Initializes a variant based on an std-string
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const std::string &value)
{
    init(D_STRING);
    data = new std::string(value);
}

/**
 * Initializes a variant based on a string (c-style)
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const char *value)
{
    init(D_STRING);
    data = new std::string(value);
}

/**
 * Initializes a variant based on a pointer
 *
 * @param value The value to use
 * @param manage True to manage the pointer memory (i.e. delete it when we
 * replace it with something else or we destruct)
 */
Variant::Variant(void *value, bool manage)
{
    init(D_POINTER);
    data = value;
    deleteData = manage;
}

/**
 * Initializes a variant based on a string vector
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const std::vector<std::string> &value)
{
    init(D_STRINGVECTOR);
    data = new std::vector<std::string>(value);
}

/**
 * Initializes a variant based on a variant vector
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const VariantVector &value)
{
    init(D_VARIANTVECTOR);
    data = new VariantVector(value);
}

/**
 * Initializes a variant based on a variant map
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const VariantMap &value)
{
    init(D_VARIANTMAP);
    data = new VariantMap(value);
}

/**
 * Initializes a variant based on a long
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const long &value)
{
    init(D_LONG);
    data = new long(value);
}

/**
 * Initializes a variant based on an unsigned long long
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const long long &value)
{
    init(D_LONGLONG);
    data = new long long(value);
}

/**
 * Initializes a variant based on an unsigned long
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const unsigned long &value)
{
    init(D_LONG);
    data = new unsigned long(value);
}

/**
 * Initializes a variant based on an unsigned long long
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const unsigned long long &value)
{
    init(D_ULONGLONG);
    data = new unsigned long long(value);
}

/**
 * Initializes a variant based on an integer
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const int &value)
{
    init(D_INT);
    data = new int(value);
}

/**
 * Initializes a variant based on an unsigned integer
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const unsigned int &value)
{
    init(D_UINT);
    data = new unsigned int(value);
}

/**
 * Initializes a variant based on a short
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const short &value)
{
    init(D_SHORT);
    data = new short(value);
}

/**
 * Initializes a variant based on an unsigned short
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const unsigned short &value)
{
    init(D_USHORT);
    data = new unsigned short(value);
}

/**
 * Initializes a variant based on a bool
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const bool &value)
{
    init(D_BOOLEAN);
    data = new bool(value);
}

/**
 * Initializes a variant based on a double
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const double &value)
{
    init(D_DOUBLE);
    data = new double(value);
}

/**
 * Initializes a variant based on a float
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const float &value)
{
    init(D_FLOAT);
    data = new float(value);
}

/**
 * Initializes a variant based on a QueryResult structure.
 *
 * @param value The object to copy
 * @return void
 */
Variant::Variant(const QueryResult &value)
{
    init(D_QUERYRESULT);
    data = new QueryResult(value);
}

/**
 *
 * Assigns the contents of the argument passed (value) to this object. This
 * makes a deep copy.
 *
 * @param value The value to copy
 * @return void
 */
void Variant::operator=(const Variant &value)
{
    if (deleteData && data != value.data) {

        // Free memory. Convert to char * to clean up compiler warnings
        delete reinterpret_cast<char *>(data);
    }

    init(value.type);
    switch (type) {
    case D_NULL:
        data = nullptr;
        break;
    case D_POINTER:
        data = value.data;
        deleteData = value.deleteData;
        break;
    case D_STRING:
        data = new std::string(value.toString());
        break;
    case D_STRINGVECTOR:
        data = new std::vector<std::string>(value.toStringVector());
        break;
    case D_VARIANTVECTOR:
        data = new VariantVector(value.toVariantVector());
        break;
    case D_VARIANTMAP:
        data = new VariantMap(value.toVariantMap());
        break;
    case D_LONG:
        data = new long(value.toLong());
        break;
    case D_LONGLONG:
        data = new unsigned long long(value.toLong());
        break;
    case D_ULONG:
        data = new unsigned long(value.toLong());
        break;
    case D_ULONGLONG:
        data = new unsigned long long(value.toLong());
        break;
    case D_INT:
        data = new int(value.toLong());
        break;
    case D_UINT:
        data = new unsigned int(value.toLong());
        break;
    case D_SHORT:
        data = new short(value.toLong());
        break;
    case D_USHORT:
        data = new unsigned short(value.toLong());
        break;
    case D_BOOLEAN:
        data = new bool(value.toBool());
        break;
    case D_DOUBLE:
        data = new double(value.toDouble());
        break;
    case D_FLOAT:
        data = new float(value.toFloat());
        break;
    case D_QUERYRESULT:
        data = new QueryResult(value.toQueryResult());
        break;
    }
}

/**
 * Compares this variant with the one identified by value. Returns true if they
 * are the same (comparison based on the type of this object). Else false.
 *
 * @param value The value to compare
 * @return True if this object equals value, else false
 */
bool Variant::operator==(const Variant &value) const
{
    DataType type;

    // By default use our type
    type = this->type;

    // Because of precision differences, we need to treat double-float
    // comparisons specially. Realistically, this comparison shouldn't ever
    // happen >.<
    if (type == D_FLOAT && value.type == D_DOUBLE) {

        return fabs(toFloat() - value.toDouble()) < 0.00001;
    } else if (type == D_DOUBLE && value.type == D_FLOAT) {

        return fabs(toDouble() - value.toFloat()) < 0.00001;
    }

    // We need to make sure that if a equals b, that b also equals a. To do this
    // it is necessary to always use the greatest (or least) type. However, it
    // is impractical to prefer string over double or float as that would make
    // 1.0 not equal "1", so decimal types are always greater than non-decimal
    // types. NONE is a special case and is not simply put at the end of the
    // DataType list because if types get added in the future, that would change
    // the id of it and break any saved server configs.
    if (type != D_NULL && (value.type == D_NULL || value.type > type)) {

        // Set type to the right side's type (greater than left or NONE)
        type = value.type;
    }

    switch (type) {
    case D_POINTER:
        return data == value.data;
    case D_STRING:
        return toString() == value.toString();
    case D_STRINGVECTOR:
        return toStringVector() == value.toStringVector();
    case D_VARIANTVECTOR:
        return toVariantVector() == value.toVariantVector();
    case D_VARIANTMAP:
        return toVariantMap() == value.toVariantMap();
    case D_DOUBLE:
        return toDouble() == value.toDouble();
    case D_FLOAT:
        return toFloat() == value.toFloat();
    case D_SHORT:
        return toShort() == value.toShort();
    case D_USHORT:
        return toUShort() == value.toUShort();
    case D_BOOLEAN:
        return toBool() == value.toBool();
    case D_INT:
        return toInt() == value.toInt();
    case D_UINT:
        return toUInt() == value.toUInt();
    case D_LONG:
        return toLong() == value.toLong();
    case D_LONGLONG:
        return toLongLong() == value.toLongLong();
    case D_ULONG:
        return toULong() == value.toULong();
    case D_ULONGLONG:
        return toULongLong() == value.toULongLong();
    case D_QUERYRESULT:
        return toQueryResult().uid == value.toQueryResult().uid;
    case D_NULL:
        return isNull() == value.isNull();
    }
}

/**
 * Compares this variant with the one identified by value. Returns true if this
 * is greater than value (comparison based on the type of this object). Else
 * false.
 *
 * @param value The value to compare
 * @return True if this object does not equal value, else false
 */
bool Variant::operator>(const Variant &value) const
{
    DataType type;

    // By default use our type
    type = this->type;

    // We need to make sure that if a > b, that b is < a. To do this
    // it is necessary to always use the greatest (or least) type -- it need
    // only be consistent
    if (value.type > this->type) {
        type = value.type;
    }

    switch (type) {
    case D_POINTER:
        return data > value.data;
    case D_STRING:
        return toString() > value.toString();
    case D_STRINGVECTOR:
        return toStringVector() > value.toStringVector();
    case D_VARIANTVECTOR:
        return toVariantVector() > value.toVariantVector();
    case D_VARIANTMAP:
        return toVariantMap() > value.toVariantMap();
    case D_DOUBLE:
        return toDouble() > value.toDouble();
    case D_FLOAT:
        return toFloat() > value.toFloat();
    case D_SHORT:
        return toShort() > value.toShort();
    case D_USHORT:
        return toUShort() > value.toUShort();
    case D_BOOLEAN:
        return toBool() > value.toBool();
    case D_INT:
        return toInt() > value.toInt();
    case D_UINT:
        return toUInt() > value.toUInt();
    case D_LONG:
        return toLong() > value.toLong();
    case D_LONGLONG:
        return toLongLong() > value.toLongLong();
    case D_ULONG:
        return toULong() > value.toULong();
    case D_ULONGLONG:
        return toULongLong() > value.toULongLong();
    case D_QUERYRESULT:
        return toQueryResult().uid > value.toQueryResult().uid;
    case D_NULL:
        return value.isNull();
    }
}

/**
 * Compares this variant with the one identified by value. Returns true if this
 * is less than value (comparison based on the type of this object). Else false.
 *
 * @param value The value to compare
 * @return True if this object is less than value, else false
 */
bool Variant::operator<(const Variant &value) const
{
    DataType type;

    // By default use our type
    type = this->type;

    // We need to make sure that if a > b, that b is < a. To do this
    // it is necessary to always use the greatest (or least) type -- it need
    // only be consistent
    if (value.type > this->type) {
        type = value.type;
    }

    switch (type) {
    case D_POINTER:
        return data < value.data;
    case D_STRING:
        return toString() < value.toString();
    case D_STRINGVECTOR:
        return toStringVector() < value.toStringVector();
    case D_VARIANTVECTOR:
        return toVariantVector() < value.toVariantVector();
    case D_VARIANTMAP:
        return toVariantMap() < value.toVariantMap();
    case D_DOUBLE:
        return toDouble() < value.toDouble();
    case D_FLOAT:
        return toFloat() < value.toFloat();
    case D_SHORT:
        return toShort() < value.toShort();
    case D_USHORT:
        return toUShort() < value.toUShort();
    case D_BOOLEAN:
        return toBool() < value.toBool();
    case D_INT:
        return toInt() < value.toInt();
    case D_UINT:
        return toUInt() < value.toUInt();
    case D_LONG:
        return toLong() < value.toLong();
    case D_LONGLONG:
        return toLongLong() < value.toLongLong();
    case D_ULONG:
        return toULong() < value.toULong();
    case D_ULONGLONG:
        return toULongLong() < value.toULongLong();
    case D_QUERYRESULT:
        return toQueryResult().uid < value.toQueryResult().uid;
    case D_NULL:
        return value.isNull();
    }
}

/**
 * Compares this variant with the one identified by value. Returns true if this
 * is greater than or equal to value (based on their data as converted to the
 * type of this object). Else false.
 *
 * @param value The value to compare
 * @return True if this object is greater than value, else false
 */
bool Variant::operator!=(const Variant &value) const
{
    return !(*this == value);
}

/**
 * Compares this variant with the one identified by value. Returns true if this
 * is less than or equal to value (based on their data as converted to the type
 * of this object). Else false.
 *
 * @param value The value to compare
 * @return True if this object is greater than or equal to value, else false
 */
bool Variant::operator>=(const Variant &value) const
{
    return (*this == value || (*this) > value);
}

/**
 * Compares this variant with the one identified by value. Returns true if this
 * is greater than or equal to value (their data comparisons return true).
 * Else false.
 *
 * @param value The value to compare
 * @return True if this object is less than or equal to value, else false
 */
bool Variant::operator<=(const Variant &value) const
{
    return (*this == value || (*this) < value);
}

/**
 *
 * Checks if this variant represents nullptr. Returns true if so else false.
 *
 * @return bool
 */
const bool Variant::isNull() const
{
    return type == D_NULL || data == nullptr;
}

/**
 *
 * Converts this variant to a string. In the event if a vector, the first
 * element will be returned.
 *
 * @return The string representation of this variant
 */
const std::string Variant::toString() const
{
    std::stringstream stream;

    switch (type) {
    case D_POINTER:
        return "";
    case D_STRING:
        return *static_cast<std::string *>(data);
    case D_VARIANTVECTOR:
    {
        VariantVector vector(*static_cast<VariantVector *>(data));
        if (vector.size() == 0) {
            return "";
        }
        return vector.at(0).toString();
    }
    case D_STRINGVECTOR:
    {
        std::vector<std::string> vector(
                    *static_cast<std::vector<std::string> *>(data));
        if (vector.size() == 0) {
            return "";
        }
        return vector.at(0);
    }
    case D_DOUBLE:
        stream << *static_cast<double *>(data);
        return stream.str();
    case D_FLOAT:
        stream << *static_cast<float *>(data);
        return stream.str();
    case D_SHORT:
        stream << *static_cast<short *>(data);
        return stream.str();
    case D_USHORT:
        stream << *static_cast<unsigned short *>(data);
        return stream.str();
    case D_BOOLEAN:
        if (*static_cast<bool *>(data)) {
            return "true";
        } else {
            return "false";
        }
    case D_INT:
        stream << *static_cast<int *>(data);
        return stream.str();
    case D_UINT:
        stream << *static_cast<unsigned int *>(data);
        return stream.str();
    case D_LONG:
        stream << *static_cast<long *>(data);
        return stream.str();
    case D_ULONG:
        stream << *static_cast<unsigned long *>(data);
        return stream.str();
    case D_NULL:
    case D_VARIANTMAP:
    default:
        return "";
    }
}

/**
 *
 * Converts this variant to a string vector. If the data happens to be a variant
 * vector, it is iterated and each element converted. Other data types will be
 * inserted into a new vector as the first element.
 *
 * @return The string vector representation of this variant
 */
const std::vector<std::string> Variant::toStringVector() const
{
    std::vector<std::string> vector;

    switch (type) {
    case D_STRINGVECTOR:
        return *static_cast<std::vector<std::string> *>(data);
    case D_VARIANTVECTOR:
    {
        VariantVector variantVector = *static_cast<VariantVector *>(data);
        for (VariantVector::iterator it = variantVector.begin();
                it != variantVector.end(); ++it) {
            vector.push_back((*it).toString());
        }
        break;
    }
    case D_POINTER:
    case D_NULL:
    case D_VARIANTMAP:
        break;
    default:
        vector.push_back(toString());
        break;
    }

    return vector;
}

/**
 *
 * Converts this variant to a variant vector. If the data happens to be a string
 * vector, it is iterated and each element converted. Other data types will be
 * inserted into a new vector as the first element.
 *
 * @return The variant vector representation of this variant
 */
const VariantVector Variant::toVariantVector() const
{
    VariantVector vector;

    switch (type) {
    case D_VARIANTVECTOR:
        return *static_cast<VariantVector *>(data);
    case D_STRINGVECTOR:
    {
        std::vector<std::string> stringVector = *static_cast<std::vector<
                std::string> *>(data);
        for (std::vector<std::string>::iterator it = stringVector.begin();
                it != stringVector.end(); ++it) {
            vector.push_back(Variant(*it));
        }
        break;
    }
    case D_POINTER:
    case D_NULL:
    case D_VARIANTMAP:
        break;
    default:
        vector.push_back(*this);
        break;
    }

    return vector;
}

/**
 *
 * Converts this variant to a float. In the event if a vector, the first
 * element will be returned.
 *
 * @return The float representation of this object
 */
const float Variant::toFloat() const
{
    return numericCast<float>();
}

/**
 *
 * Converts this variant to a double. In the event if a vector, the first
 * element will be returned.
 *
 * @return The double representation of this object
 */
const double Variant::toDouble() const
{
    return numericCast<double>();
}

/**
 *
 * Converts this variant to a long. In the event if a vector, the first
 * element will be returned.
 *
 * @return The long representation of this object
 */
const long Variant::toLong() const
{
    return numericCast<long>();
}

/**
 *
 * Converts this variant to a long long. In the event if a vector, the first
 * element will be returned.
 *
 * @return The long representation of this object
 */
const long long Variant::toLongLong() const
{
    return numericCast<long long>();
}

/**
 *
 * Converts this variant to an unsigned long. In the event if a vector, the
 * first element will be returned.
 *
 * @return The unsigned long representation of this object
 */
const unsigned long Variant::toULong() const
{
    return numericCast<unsigned long>();
}

/**
 *
 * Converts this variant to an unsigned long long. In the event if a vector, the
 * first element will be returned.
 *
 * @return The unsigned long representation of this object
 */
const unsigned long long Variant::toULongLong() const
{
    return numericCast<unsigned long long>();
}

/**
 *
 * Converts this variant to an integer. In the event if a vector, the first
 * element will be returned.
 *
 * @return The int representation of this object
 */
const int Variant::toInt() const
{
    return numericCast<int>();
}

/**
 *
 * Converts this variant to an unsigned integer. In the event if a vector, the
 * first element will be returned.
 *
 * @return The unsigned int representation of this object
 */
const unsigned int Variant::toUInt() const
{
    return numericCast<unsigned int>();
}

/**
 *
 * Converts this variant to a short. In the event if a vector, the first element
 * will be returned.
 *
 * @return The short representation of this object
 */
const short Variant::toShort() const
{
    return numericCast<short>();
}

/**
 *
 * Converts this variant to an unsigned short. In the event if a vector, the
 * first element will be returned.
 *
 * @return The unsigned short representation of this object
 */
const unsigned short Variant::toUShort() const
{
    return numericCast<unsigned short>();
}

/**
 *
 * Converts this variant to a bool. In the event if a vector, the first element
 * will be returned.
 *
 * @return The unsigned short representation of this object
 */
const bool Variant::toBool() const
{
    if (type == D_BOOLEAN) {

        // Cast to bool
        return numericCast<bool>();
    }
    auto data = toString();

    // Convert to lowercase
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);

    if (data == "true") {

        return true;
    } else {

        // Return true if numeric cast evaluates to true
        return numericCast<bool>();
    }
}

/**
 *
 * Converts this variant to a query result. If the stored type is not a query
 * result, returns an empty QueryResult
 *
 * @return The QueryResult representation of this object
 */
const QueryResult Variant::toQueryResult() const
{
    if (type == D_QUERYRESULT) {

        return *static_cast<QueryResult *>(data);
    }

    return QueryResult();
}

/**
 *
 * Converts this variant to a variant map. If the stored type is not a variant
 * map, returns an empty variant map
 *
 * @return The variant map representation of this object
 */
const VariantMap Variant::toVariantMap() const
{
    if (type == D_VARIANTMAP) {

        return *static_cast<VariantMap *>(data);
    }

    return VariantMap();
}

/**
 *
 * Converts our data to the type identified by T
 *
 * @return a numeric-type cast of data
 */
template<typename T>
T Variant::numericCast() const
{
    switch (type) {
    case D_STRING:
    case D_STRINGVECTOR:
    case D_VARIANTVECTOR:
    {
        T number;
        std::istringstream stream(toString());
        stream >> number;
        return number;
    }
    case D_DOUBLE:
        return *static_cast<double *>(data);
    case D_FLOAT:
        return *static_cast<float *>(data);
    case D_SHORT:
        return *static_cast<short *>(data);
    case D_USHORT:
        return *static_cast<unsigned short *>(data);
    case D_INT:
        return *static_cast<int *>(data);
    case D_UINT:
        return *static_cast<unsigned int *>(data);
    case D_LONG:
        return *static_cast<long *>(data);
    case D_ULONG:
        return *static_cast<unsigned long *>(data);
    case D_NULL:
    case D_POINTER:
    default:
        return 0;
    }
}

/**
 *
 * Returns the type of this variant
 *
 * @return The type
 */
const DataType Variant::getType() const
{
    return type;
}

/**
 *
 * Writes our data to the provided variant list.
 *
 * @param variant A variant object
 * @return The original variant
 */
VariantVector &VariantVector::operator<<(const Variant &variant)
{
    // Add to collection
    push_back(variant);

    // Return the list
    return *this;
}

/**
 *
 * Converts this to a Variant and returns the newly created object
 *
 * @return Variant the return variant
 */
const Variant VariantVector::toVariant()
{
    return Variant(*this);
}

/**
 * This object is being destroyed. Lets free up the memory used by our internal
 * data
 *
 * @return void
 */
Variant::~Variant()
{
    if (deleteData) {

        // Free memory. Convert to char * to clean up compiler warnings
        delete reinterpret_cast<char *>(data);
    }
}

} // namespace RabidSQL


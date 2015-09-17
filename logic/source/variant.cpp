#include "app.h"
#include "filestream.h"
#include "variant.h"
#include "structs.h"

#include <cmath>

namespace RabidSQL {

/**
 * Initializes a null variant.
 *
 * @return void
 */
Variant::Variant()
{
    type = NONE;
    data = nullptr;
}

/**
 * Initializes a null variant
 *
 * @return void
 */
Variant::Variant(const std::nullptr_t &value)
{
    type = NONE;
    data = nullptr;
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
    type = NONE;
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
    type = STRING;
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
    type = STRING;
    data = new std::string(value);
}

/**
 * Initializes a variant based on a string vector
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const std::vector<std::string> &value)
{
    type = STRINGVECTOR;
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
    type = VARIANTVECTOR;
    data = new VariantVector(value);
}

/**
 * Initializes a variant based on a long
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const long &value)
{
    type = LONG;
    data = new long(value);
}

/**
 * Initializes a variant based on an unsigned long
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const unsigned long &value)
{
    type = ULONG;
    data = new unsigned long(value);
}

/**
 * Initializes a variant based on an integer
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const int &value)
{
    type = INT;
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
    type = UINT;
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
    type = SHORT;
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
    type = USHORT;
    data = new unsigned short(value);
}

/**
 * Initializes a variant based on a double
 *
 * @param value The value to copy
 * @return void
 */
Variant::Variant(const double &value)
{
    type = DOUBLE;
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
    type = FLOAT;
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
    type = QUERYRESULT;
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
    if (type != NONE) {

        // Free memory. Convert to char * to clean up compiler warnings
        delete reinterpret_cast<char *>(data);
    }

    type = value.type;
    switch (type) {
    case NONE:
        data = nullptr;
        break;
    case STRING:
        data = new std::string(value.toString());
        break;
    case STRINGVECTOR:
        data = new std::vector<std::string>(value.toStringVector());
        break;
    case VARIANTVECTOR:
        data = new VariantVector(value.toVariantVector());
        break;
    case LONG:
        data = new long(value.toLong());
        break;
    case ULONG:
        data = new unsigned long(value.toLong());
        break;
    case INT:
        data = new int(value.toLong());
        break;
    case UINT:
        data = new unsigned int(value.toLong());
        break;
    case SHORT:
        data = new short(value.toLong());
        break;
    case USHORT:
        data = new unsigned short(value.toLong());
        break;
    case DOUBLE:
        data = new double(value.toDouble());
        break;
    case FLOAT:
        data = new float(value.toFloat());
        break;
    case QUERYRESULT:
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
    if (type == FLOAT && value.type == DOUBLE) {

        return fabs(toFloat() - value.toDouble()) < 0.00001;
    } else if (type == DOUBLE && value.type == FLOAT) {

        return fabs(toDouble() - value.toFloat()) < 0.00001;
    }

    // We need to make sure that if a equals b, that b also equals a. To do this
    // it is necessary to always use the greatest (or least) type. However, it
    // is impractical to prefer string over double or float as that would make
    // 1.0 not equal "1", so decimal types are always greater than non-decimal
    // types. NONE is a special case and is not simply put at the end of the
    // DataType list because if types get added in the future, that would change
    // the id of it and break any saved server configs.
    if (type != NONE && (value.type == NONE || value.type > type)) {

        // Set type to the right side's type (greater than left or NONE)
        type = value.type;
    }

    switch (type) {
    case STRING:
        return toString() == value.toString();
    case STRINGVECTOR:
        return toStringVector() == value.toStringVector();
    case VARIANTVECTOR:
        return toVariantVector() == value.toVariantVector();
    case DOUBLE:
        return toDouble() == value.toDouble();
    case FLOAT:
        return toFloat() == value.toFloat();
    case SHORT:
        return toShort() == value.toShort();
    case USHORT:
        return toUShort() == value.toUShort();
    case INT:
        return toInt() == value.toInt();
    case UINT:
        return toUInt() == value.toUInt();
    case LONG:
        return toLong() == value.toLong();
    case ULONG:
        return toULong() == value.toULong();
    case QUERYRESULT:
        return toQueryResult().uid == value.toQueryResult().uid;
    case NONE:
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
    case STRING:
        return toString() > value.toString();
    case STRINGVECTOR:
        return toStringVector() > value.toStringVector();
    case VARIANTVECTOR:
        return toVariantVector() > value.toVariantVector();
    case DOUBLE:
        return toDouble() > value.toDouble();
    case FLOAT:
        return toFloat() > value.toFloat();
    case SHORT:
        return toShort() > value.toShort();
    case USHORT:
        return toUShort() > value.toUShort();
    case INT:
        return toInt() > value.toInt();
    case UINT:
        return toUInt() > value.toUInt();
    case LONG:
        return toLong() > value.toLong();
    case ULONG:
        return toULong() > value.toULong();
    case QUERYRESULT:
        return toQueryResult().uid > value.toQueryResult().uid;
    case NONE:
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
    case STRING:
        return toString() < value.toString();
    case STRINGVECTOR:
        return toStringVector() < value.toStringVector();
    case VARIANTVECTOR:
        return toVariantVector() < value.toVariantVector();
    case DOUBLE:
        return toDouble() < value.toDouble();
    case FLOAT:
        return toFloat() < value.toFloat();
    case SHORT:
        return toShort() < value.toShort();
    case USHORT:
        return toUShort() < value.toUShort();
    case INT:
        return toInt() < value.toInt();
    case UINT:
        return toUInt() < value.toUInt();
    case LONG:
        return toLong() < value.toLong();
    case ULONG:
        return toULong() < value.toULong();
    case QUERYRESULT:
        return toQueryResult().uid < value.toQueryResult().uid;
    case NONE:
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
 * Checks if this variant represents nullptr. Returns true if so else false
 *
 * @return bool
 */
const bool Variant::isNull() const
{
    return type == NONE;
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
    case STRING:
        return *static_cast<std::string *>(data);
    case VARIANTVECTOR:
    {
        VariantVector vector(*static_cast<VariantVector *>(data));
        if (vector.size() == 0) {
            return "";
        }
        return vector.at(0).toString();
    }
    case STRINGVECTOR:
    {
        std::vector<std::string> vector(
                    *static_cast<std::vector<std::string> *>(data));
        if (vector.size() == 0) {
            return "";
        }
        return vector.at(0);
    }
    case DOUBLE:
        stream << *static_cast<double *>(data);
        return stream.str();
    case FLOAT:
        stream << *static_cast<float *>(data);
        return stream.str();
    case SHORT:
        stream << *static_cast<short *>(data);
        return stream.str();
    case USHORT:
        stream << *static_cast<unsigned short *>(data);
        return stream.str();
    case INT:
        stream << *static_cast<int *>(data);
        return stream.str();
    case UINT:
        stream << *static_cast<unsigned int *>(data);
        return stream.str();
    case LONG:
        stream << *static_cast<long *>(data);
        return stream.str();
    case ULONG:
        stream << *static_cast<unsigned long *>(data);
        return stream.str();
    case NONE:
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
    case STRINGVECTOR:
        return *static_cast<std::vector<std::string> *>(data);
    case VARIANTVECTOR:
    {
        VariantVector variantVector = *static_cast<VariantVector *>(data);
        for (VariantVector::iterator it = variantVector.begin();
                it != variantVector.end(); ++it) {
            vector.push_back((*it).toString());
        }
        break;
    }
    case NONE:
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
    case VARIANTVECTOR:
        return *static_cast<VariantVector *>(data);
    case STRINGVECTOR:
    {
        std::vector<std::string> stringVector = *static_cast<std::vector<
                std::string> *>(data);
        for (std::vector<std::string>::iterator it = stringVector.begin();
                it != stringVector.end(); ++it) {
            vector.push_back(Variant(*it));
        }
        break;
    }
    case NONE:
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
 * Converts this variant to a query result. If the stored type is not a query
 * result, returns an empty QueryResult
 *
 * @return The QueryResult representation of this object
 */
const QueryResult &Variant::toQueryResult() const
{
    if (type == QUERYRESULT) {

        return *static_cast<QueryResult *>(data);
    }

    return *new QueryResult();
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
    case STRING:
    case STRINGVECTOR:
    case VARIANTVECTOR:
    {
        T number;
        std::istringstream stream(toString());
        stream >> number;
        return number;
    }
    case DOUBLE:
        return *static_cast<double *>(data);
    case FLOAT:
        return *static_cast<float *>(data);
    case SHORT:
        return *static_cast<short *>(data);
    case USHORT:
        return *static_cast<unsigned short *>(data);
    case INT:
        return *static_cast<int *>(data);
    case UINT:
        return *static_cast<unsigned int *>(data);
    case LONG:
        return *static_cast<long *>(data);
    case ULONG:
        return *static_cast<unsigned long *>(data);
    case NONE:
    default:
        return 0;
    }
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
    if (!isNull()) {

        // Free memory. Convert to char * to clean up compiler warnings
        delete reinterpret_cast<char *>(data);
    }
}

} // namespace RabidSQL


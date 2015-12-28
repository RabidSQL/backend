#ifndef RABIDSQL_FILESTREAM_H
#define RABIDSQL_FILESTREAM_H

#include "NSEnums.h"
#include "Variant.h"

#include <fstream>
#include <stack>

namespace rapidjson
{
    typedef unsigned SizeType;
} // namespace rapidjson

namespace RabidSQL
{

class FileStream : public std::fstream {
public:
    virtual bool open(std::string filename, std::ios_base::openmode mode);
    virtual FileFormat getFormat() = 0;
    virtual FileStream &operator<<(const Variant &value) = 0;
    virtual FileStream &operator>>(Variant &value) = 0;
};

} // namespace RabidSQL

#endif //RABIDSQL_FILESTREAM_H

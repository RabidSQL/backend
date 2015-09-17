#ifndef RABIDSQL_FILESTREAM_H
#define RABIDSQL_FILESTREAM_H

#include "structs.h"

#include <fstream>

namespace RabidSQL
{

class FileStream : public std::fstream {
public:
    virtual bool open(std::string filename, std::ios_base::openmode mode);
    virtual FileFormat::format getFormat() = 0;
    virtual FileStream &operator<<(const Variant &value) = 0;
    virtual FileStream &operator>>(Variant &value) = 0;
};

class JsonStream : virtual public FileStream {
public:
    FileFormat::format getFormat();
    virtual ~JsonStream() {}
    FileStream &operator<<(const Variant &value);
    FileStream &operator>>(Variant &value);
};

class BinaryStream : virtual public FileStream {
public:
    bool open(std::string filename, std::ios_base::openmode mode);
    void mark();
    bool expectMark();
    FileFormat::format getFormat();
    virtual ~BinaryStream() {}
    FileStream &operator<<(const Variant &value);
    FileStream &operator>>(Variant &value);
private:
    std::string readString();
};

} // namespace RabidSQL

#endif //RABIDSQL_FILESTREAM_H

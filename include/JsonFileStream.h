#ifndef RABIDSQL_JSONFILESTREAM_H
#define RABIDSQL_JSONFILESTREAM_H

#include "FileStream.h"

namespace RabidSQL {

class Variant;

class JsonFileStream : virtual public FileStream {
public:
    typedef char Ch;
    void *writer;

    RabidSQL::FileFormat getFormat();
    JsonFileStream();
    virtual ~JsonFileStream();
    FileStream &operator<<(const Variant &value);
    FileStream &operator>>(Variant &value);

    // Writer methods
    void Put(Ch);
    void Flush();
    Ch* PutBegin();
    size_t PutEnd(Ch*);

    // Reader methods
    Ch Peek() const;
    Ch Take();
    size_t Tell() const;

private:
    char current;
    unsigned int count;
};

} // namespace RabidSQL

#endif //RABIDSQL_JSONFILESTREAM_H

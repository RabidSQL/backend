#ifndef RABIDSQL_BINARYFILESTREAM_H
#define RABIDSQL_BINARYFILESTREAM_H

#include "FileStream.h"

namespace RabidSQL {

class BinaryFileStream : virtual public FileStream {
public:
    bool open(std::string filename, unsigned int mode);
    void mark();
    bool expectMark();
    FileFormat getFormat();
    virtual ~BinaryFileStream() {}
    FileStream &operator<<(const Variant &value);
    FileStream &operator>>(Variant &value);
private:
    std::string readString();
};

}

#endif //RABIDSQL_BINARYFILESTREAM_H

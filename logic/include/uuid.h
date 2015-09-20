#ifndef RABIDSQL_UUID_H
#define RABIDSQL_UUID_H

#include <mutex>
#include <random>
#include <string>

namespace RabidSQL {

class UUID {
public:
    static std::string makeUUID();
    static std::string threadSafeMakeUUID();

private:
    static std::mt19937 generator;
    static std::mutex mutex;
};

} // namespace RabidSQL

#endif //RABIDSQL_UUID_H

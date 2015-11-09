#ifndef RABIDSQL_CONSOLE_H
#define RABIDSQL_CONSOLE_H

#include <string>
#include <iostream>

namespace RabidSQL {

class Variant;
class Console
{
public:
    static Console debug();
    Console &operator<<(const Variant &value);
    Console &operator<<(const std::string &value);
    Console &operator<<(const char *value);
    Console &operator<<(const void *object);
    ~Console();

private:
    Console(std::string title, std::ostream *stream);

    unsigned int count;
    std::string title;
    std::ostream *stream;
};

} // namespace RabidSQL

#endif // RABIDSQL_CONSOLE_H

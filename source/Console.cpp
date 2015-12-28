#include "Console.h"
#include "Variant.h"

#include <iostream>
#include <sstream>

namespace RabidSQL {

/**
 *
 * Initializes a console output wrapper. The supplied title will be output
 * upon the first stream append
 *
 * @param title The title to display
 * @param stream The output stream to use
 * @return void
 */
Console::Console(std::string title, std::ostream *stream)
{
    this->title = title;
    this->stream = stream;

    count = 0;
}

/**
 *
 * Returns an instance of Console for use in debugging. This uses /dev/stdout
 *
 * @return The Console instance
 */
Console Console::debug()
{
    return Console("debug", &std::cout);
}

/**
 *
 * Append value to the data stream
 *
 * @param value The value to display
 * @return The Console instance
 */
Console &Console::operator<<(const Variant &value)
{

    // Convert the variant to a string, output it, and return our instance
    return (*this) << value.toString();
}

/**
 *
 * Append value to the data stream
 *
 * @param value The value to display
 * @return The Console instance
 */
Console &Console::operator<<(const char *value)
{

    // Convert the character array to a string, output it, and return our
    // instance
    return (*this) << std::string(value);
}

/**
 *
 * Append value to the data stream
 *
 * @param value The value to display
 * @return The Console instance
 */
Console &Console::operator<<(const void *value)
{

    // Create a string stream for manipulation
    std::stringstream stream;

    // Send the hex memory address to our temporary stream
    stream << std::hex << (long) value;

    // Output the memory address, and return our instance
    return (*this) << "0x" + stream.str();
}

/**
 *
 * Append value to the data stream
 *
 * @param value The value to display
 * @return The Console instance
 */
Console &Console::operator<<(const std::string &value)
{
    if (count++ == 0) {

        // Display the title first
        (*stream) << "[" + title + "] ";
    } else {

        // Separate each element with a space
        (*stream) << " ";
    }

    (*stream) << value;

    return (*this);
}

/**
 *
 * Outputs a newline character and flushes the stream (shutting down instance)
 *
 * @return void
 */
Console::~Console()
{
    (*stream) << std::endl;
}

} // namespace RabidSQL


#include "App.h"
#include "FileStream.h"
#include "QueryResult.h"
#include "BinaryFileStream.h"

namespace RabidSQL {

/**
 *
 * Opens the file specified in the mode specified and returns a boolean
 * indicating whether it opened successfully or not
 *
 * @param filename The filename to open
 * @param mode The mode to open the file in
 * @return bool
 */
bool FileStream::open(std::string filename, std::ios_base::openmode mode)
{
    std::fstream::open(filename, mode);

    return is_open();
}

} // namespace RabidSQL

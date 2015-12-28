#include "Message.h"

namespace RabidSQL {

/**
 * Returns false to indicate that this is a valid message, or true
 *
 * @return bool
 */
bool Message::operator!() const
{
    return type == RabidSQL::NO_MESSAGE;
}

} // namespace RabidSQL

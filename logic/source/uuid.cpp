#include "uuid.h"

namespace RabidSQL {

/**
 * Generates a unique ID and returns it as a string. Note that this is NOT
 * guaranteed to be unique across threads. Use threadSafeMakeUUID for that
 * purpose.
 *
 * @return The generated UUID
 */
std::string UUID::makeUUID() {
    char uuid[] = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    char buffer[1] = { 0 };

    for (int i = strlen(uuid) - 1; i >= 0; i--) {

        if (uuid[i] != 'x' && uuid[i] != 'y') {

            // No processing necessary
            continue;
        }
        double rand = (double) generator() / generator.max();

        switch (uuid[i]) {
            case 'x':

                // 0-f format
                sprintf(buffer, "%x", int(rand * 16));
                uuid[i] = buffer[0];
                break;
            case 'y':

                // 8-b format
                sprintf(buffer, "%x", int(rand * 4) + 8);
                uuid[i] = buffer[0];
                break;
        }
    }

    return uuid;
}

/**
 * Generates a unique ID that is guaranteed to be unique across threads and
 * returns it as a string
 *
 * @return The generated UUID
 */
std::string UUID::threadSafeMakeUUID() {

    // Lock mutex
    mutex.lock();

    std::string uuid = makeUUID();

    // Unlock mutex
    mutex.unlock();

    return uuid;
}

std::mt19937 UUID::generator(
        std::chrono::system_clock::now().time_since_epoch().count());
std::mutex UUID::mutex;

} // namespace RabidSQL
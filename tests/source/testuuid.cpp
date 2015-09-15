#include "thread.h"
#include "uuid.h"
#include "gtest/gtest.h"


namespace RabidSQL {

// Tests Unique ID uniqueness
TEST(TestUUID, Uniqueness) {

    std::vector<std::string> uuids;
    std::string uuid;

    for (int i = 0; i < 1000; i++) {

        // Generate UUID
        uuid = UUID::makeUUID();

        // Ensure unique
        ASSERT_EQ(std::find(uuids.begin(), uuids.end(), uuid), uuids.end());

        // Add to collection
        uuids.push_back(uuid);
    }
}

class TestThreadedUUIDs : virtual public Thread
{
public:
    virtual ~TestThreadedUUIDs() {}
    void run()
    {
        for (int i = 0; i < 1000; i++) {

            // Add to collection
            uuids.push_back(UUID::threadSafeMakeUUID());
        }
    }

    std::vector<std::string> uuids;
};


// Tests Unique ID uniqueness across threads
TEST(TestUUID, ThreadedUniqueness) {

    std::vector<std::string> uuids;

    // It takes lots of threads to actually conflict even with non thread-safe
    // code. 10 threads @ 1000 uids each conflicts 90% of the time using non-
    // thread-safe code, so it seems like a reasonable number. It should and
    // does conflict 0% of the time using threadSafeMakeUUID
    std::vector<TestThreadedUUIDs> threads(10);

    for (int i = 0; i < threads.size(); i++) {

        // Start threads
        threads[i].start();
    }

    for (int i = 0; i < threads.size(); i++) {

        // Wait for all the threads to finish
        threads[i].join();

        for (auto it = threads[i].uuids.begin(); it != threads[i].uuids.end();
                ++it) {

            // Ensure unique
            ASSERT_EQ(std::find(uuids.begin(), uuids.end(), *it), uuids.end());

            // Add to collection
            uuids.push_back(*it);
        }
    }
}

} // namespace RabidSQL

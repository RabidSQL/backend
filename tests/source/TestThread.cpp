#include "Thread.h"
#include "gtest/gtest.h"

#include <algorithm>

namespace RabidSQL {

class TestThreader : virtual public Thread
{
public:
    TestThreader();
    TestThreader(int delay);
    void run();
    virtual ~TestThreader();

    unsigned int delay;
    static std::mutex mutex;
    static std::vector<TestThreader *> deletedThreads;
};

std::mutex TestThreader::mutex;
std::vector<TestThreader *> TestThreader::deletedThreads;

TestThreader::TestThreader(): Thread() {
    this->delay = 0;
}

TestThreader::TestThreader(int delay) {
    this->delay = delay;
}

void TestThreader::run() {

    // Wait until finished is true
    while (!isStopping()) {

        // Sleep for 30 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    if (delay > 0) {

        // Wait for delay ms before actually shutting down
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

TestThreader::~TestThreader() {

    // Stop it!
    stop();

    // Join is also called in the parent destructor, but we want to ensure shut
    // down is actually complete before we add to our collection of shut down
    // threads
    join();

    // Lock mutex
    mutex.lock();

    // Add to collection
    // This data is used in TEST(Thread, ShutdownCompletedFromDestructor)
    deletedThreads.push_back(this);

    // Unlock mutex
    mutex.unlock();
}

// Tests that the thread has shut down successfully
TEST(TestThread, ShutdownCompleted) {

    // Initialize a thread
    TestThreader thread;

    // Start the thread
    thread.start();

    // Ensure thread is not finished
    ASSERT_FALSE(thread.isFinished());

    // Tell it to stop and block until it does so
    thread.stop(true);

    // Check if the thread has actually stopped
    ASSERT_TRUE(thread.isFinished());
}

// Tests that the thread has shut down successfully when shut down actually
// takes time
TEST(TestThread, ShutdownCompletedAfterTime) {

    // Initialize a thread with a 15 ms shutdown time
    TestThreader thread(15);

    // Start the thread
    thread.start();

    // Sleep for 10 ms to ensure that the thread has actually started running
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Tell the thread to stop
    thread.stop(false);

    // The thread takes at least 15 ms to shut down, so it should still be
    // running
    ASSERT_FALSE(thread.isFinished());

    // Sleep for (our shutdown time [15 ms] + the thread's loop sleep time
    // [30 ms]) * for good measure [3]
    std::this_thread::sleep_for(std::chrono::milliseconds((30 + 15) * 3));

    // The thread should definitely be finished now
    ASSERT_TRUE(thread.isFinished());
}

// Tests that the thread has shut down successfully when shut down by the
// application. This is to ensure that an infinite loop isn't created on a child
// thread.
TEST(TestThread, ShutdownCompletedFromDestructor) {

    // Initialize a thread
    TestThreader *thread = new TestThreader();

    // Start it up
    thread->start();

    // Shut down thread
    delete thread;

    // Lock mutex
    TestThreader::mutex.lock();

    // Make a copy of the deleted threads vector
    auto vector = TestThreader::deletedThreads;

    // Unlock mutex
    TestThreader::mutex.unlock();

    // Ensure that the thread in question was in fact shut down
    ASSERT_NE(std::find(vector.begin(), vector.end(), thread), vector.end());
}

} // namespace RabidSQL


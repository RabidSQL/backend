#include "thread.h"
#include "threadlocal.h"
#include "gtest/gtest.h"

namespace RabidSQL {

class ThreadLocalTester : virtual public Thread
{
public:
    enum Command {
        NONE,
        SETDATA,
        GETDATA
    };

    ThreadLocalTester();
    void run();
    virtual ~ThreadLocalTester();
    int get();
    void set(int data);

    std::atomic_int setData;
    std::atomic_int getData;
    ThreadLocal<int> data;
    std::atomic<Command> command;
};

ThreadLocalTester::ThreadLocalTester(): Thread()
{
    setData = -1;
    getData = -1;
    command = NONE;
}

void ThreadLocalTester::run() {

    // Wait until finished is true
    while (!isStopping()) {

        Command command = this->command;

        switch (Command(this->command)) {
            case SETDATA:
                data.set(setData);
                // Reset to no command
                this->command = NONE;
                break;
            case GETDATA:
                getData = *data.get();
                // Reset to no command
                this->command = NONE;
                break;
            case NONE:
                // Sleep for 30 ms
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
                break;
        }
    }
}

int ThreadLocalTester::get()
{
    this->command = GETDATA;

    // Wait until finished is true
    while (this->command != NONE && !isFinished()) {

        // Sleep for 30 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    return getData;
}

void ThreadLocalTester::set(int data)
{
    this->setData = data;
    this->command = SETDATA;

    // Wait until finished is true
    while (this->command != NONE && !isFinished()) {

        // Sleep for 30 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

ThreadLocalTester::~ThreadLocalTester()
{
}

// Tests that the threadlocal data is unique between threads
TEST(TestThreadLocal, MainThread) {

    // Initialize ThreadLocal storage
    ThreadLocal<int> data;

    // Set value to 123
    data.set(123);

    // Confirm that it equals
    ASSERT_EQ(*data.get(), 123);
}

// Tests that the threadlocal data is unique between threads
TEST(TestThreadLocal, MultiThread) {

    // Initialize a thread and set its storage to 1
    ThreadLocalTester thread;

    // Set in main thread
    thread.data.set(1);

    // Start thread
    thread.start();

    // Test that our local data is still 1
    ASSERT_EQ(1, *thread.data.get());

    // Set the data within the thread to 2
    thread.set(2);

    // Test that our local data remains 1
    ASSERT_EQ(1, *thread.data.get());

    // Test that the thread's data is 2
    ASSERT_EQ(2, thread.get());

    // Set the data within the thread to 3
    thread.set(3);

    // Set in main thread again
    thread.data.set(2);

    // Test that the main thread still contains 2
    ASSERT_EQ(2, *thread.data.get());

    // Test that the child thread contains 3
    ASSERT_EQ(3, thread.get());

    // Test that the main thread still contains 2
    ASSERT_EQ(2, *thread.data.get());
}

} // namespace RabidSQL


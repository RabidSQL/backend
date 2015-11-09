#include "app.h"
#include "thread.h"

#include <future>

namespace RabidSQL {

/**
 *
 * Initializes a new Thread object
 *
 * @return void
 */
Thread::Thread()
{
    #ifdef TRACK_POINTERS
    rDebug << "Thread::construct" << this;
    #endif

    // Initialize thread
    thread = nullptr;

    // Mark stopping as false
    stopping = false;

    // Mark as unfinished
    finished = false;
}

/**
 *
 * Starts thread execution
 *
 * @return void
 */
void Thread::start()
{
    if (thread != nullptr) {

        #ifdef DEBUG
        rDebug << "Attempt to start a thread that was already running!";
        #endif

        return;
    }

    thread = new std::thread(&Thread::_run, this);
}

/**
 *
 * Runs the thread and cleanup tasks
 *
 * @return void
 */
void Thread::_run()
{
    // Increment thread counter
    activeThreads++;

    // Run thread's event loop
    run();

    // Decrement thread counter
    activeThreads--;

    finished = true;
}

/**
 *
 * Checks if this thread has finished running. The returns true if the thread
 * is not finished or has not started. Otherwise returns false
 *
 * @return The state of this thread
 */
bool Thread::isFinished()
{
    if (thread == nullptr) {

        // If it hasn't started it technically hasn't finished but for our
        // intent, it has.
        return true;
    }

    return finished;
}

/**
 *
 * Joins on this thread if it is running. If not, this does nothing
 *
 * @return void
 */
void Thread::join()
{
    if (thread == nullptr) {

        // Nothing to do if the thread isn't running
        return;
    }

    #ifdef DEBUG
    if (thread->joinable()) {
        rDebug << "Thread is not joinable:" << this;
    }
    #endif

    if (thread->joinable()) {

        // Join if it is joinable
        thread->join();
    }
}

/**
 *
 * Sets this thread as "stopping". It is up to the subclass to actually handle
 * shutdown
 *
 * @return void
 */
void Thread::stop(bool block)
{

    // Mark this thread as stopping
    stopping = true;

    if (block) {

        // Wait for the thread to finish
        join();
    }
}

/**
 *
 * Returns true if the thread is in the process of stopping, else false
 *
 * @return bool
 */
bool Thread::isStopping()
{
    return stopping;
}

/**
 *
 * Returns the number of active threads.
 *
 * @return the number of active threads
 */
int Thread::numberOfActiveThreads()
{
    return activeThreads;
}

/**
 *
 * Waits for the thread to shut down and then does any cleanup tasks
 *
 * @return void
 */
Thread::~Thread()
{
    #ifdef TRACK_POINTERS
    rDebug << "Thread::destruct" << this;
    #endif

    stop();

    if (thread != nullptr) {

        // Block until finished
        join();

        // Free memory
        delete thread;
    }
}

std::atomic_int Thread::activeThreads(0);

} // namespace RabidSQL


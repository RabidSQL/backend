#ifndef RABIDSQL_THREAD_H
#define RABIDSQL_THREAD_H

#include "smartobject.h"

#include <future>

namespace RabidSQL {

class Thread : public SmartObject
{
public:
    Thread();
    virtual void run() = 0;
    void start();
    void stop(bool block = true);
    void join();

    bool isFinished();
    bool isStopping();

    ~Thread();

    static void processQueue();
    static int numberOfActiveThreads();

private:
    void _run();

    std::thread *thread;
    std::atomic_bool finished;
    std::atomic_bool stopping;
    static std::atomic_int activeThreads;
};

} // namespace RabidSQL

#endif // RABIDSQL_THREAD_H

#ifndef RABIDSQL_THREAD_H
#define RABIDSQL_THREAD_H

#include "SmartObject.h"

#include <future>

namespace RabidSQL {

class Thread : public SmartObject
{
public:
    Thread();
    virtual void start();
    virtual void stop(bool block = true);
    virtual void join();

    bool isFinished();
    bool isStopping();

    ~Thread();

    static int numberOfActiveThreads();

protected:
    virtual void run() = 0;

private:
    void _run();

    std::thread *thread;
    std::atomic_bool finished;
    std::atomic_bool stopping;
    static std::atomic_int activeThreads;
};

} // namespace RabidSQL

#endif // RABIDSQL_THREAD_H

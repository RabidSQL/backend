#ifndef RABIDSQL_THREADLOCAL_H
#define RABIDSQL_THREADLOCAL_H

#include "app.h"
#include <map>
#include <mutex>
#include <thread>

namespace RabidSQL {

// Local version of thread_local
template<class T>
class ThreadLocal
{
public:
    T *get();
    void set(T value);
    void set(T *value);
    ~ThreadLocal();

private:
    std::map<std::thread::id, T *> object;
    std::vector<T *> createdObjects;
    std::mutex mutex;
};

/**
 *
 * Gets an object of type T unique to this thread
 *
 * @return The local object
 */
template <class T>
T *ThreadLocal<T>::get()
{
    auto thread_id = std::this_thread::get_id();

    // Lock mutex
    mutex.lock();

    auto it = object.find(thread_id);

    if (it == object.end()) {

        // Initialize a new object
        auto local = new T();

        // Add to created objects collection
        createdObjects.push_back(local);

        // Add to collection
        object[thread_id] = local;

        // Unlock mutex
        mutex.unlock();

        return local;
    } else {

        // Unlock mutex
        mutex.unlock();

        // Get object
        return it->second;
    }
}

/**
 *
 * Gets an object of type T unique to this thread
 *
 * @return The local object
 */
template <class T>
void ThreadLocal<T>::set(T *value)
{
    auto thread_id = std::this_thread::get_id();

    // Lock mutex
    mutex.lock();

    object[thread_id] = value;

    // Unlock mutex
    mutex.unlock();
}

/**
 *
 * Gets an object of type T unique to this thread
 *
 * @return The local object
 */
template <class T>
void ThreadLocal<T>::set(T value)
{
    auto thread_id = std::this_thread::get_id();

    // Lock mutex
    mutex.lock();

    // Add to collection
    object[thread_id] = new T(value);

    // Add to created objects collection
    createdObjects.push_back(object[thread_id]);

    // Unlock mutex
    mutex.unlock();
}

/**
 *
 * Destroys our internal pointers
 *
 * @return void
 */
template <class T>
ThreadLocal<T>::~ThreadLocal()
{
    // Lock mutex
    mutex.lock();

    for (auto it = createdObjects.begin(); it != createdObjects.end(); ++it) {

        // Free memory
        delete *it;
    }

    // Remove elements
    createdObjects.clear();

    // Remove elements
    object.clear();

    // Unlock mutex
    mutex.unlock();
}

} // namespace RabidSQL

#endif // RABIDSQL_THREADLOCAL_H

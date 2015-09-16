#include "smartobjecttester.h"

namespace RabidSQL {

// This class is used for testing smart object destruction. Because the
// Application deals with some SmartObject pointers, this is necessary
// both for testing Application functionality and SmartObject functionality.
// It is also used for testing the database connection manager data processing

/**
 *
 * Initializes the tester class, adding it to the objects collection
 *
 * @return void
 */
SmartObjectTester::SmartObjectTester(SmartObject *parent) :
        SmartObject(parent)
{
        // Add to collection
        objects.push_back(this);
}

/**
 *
 * Removes the tester class from the object collection
 *
 * @return void
 */
SmartObjectTester::~SmartObjectTester()
{
    auto it = std::find(objects.begin(), objects.end(), this);

    if (it != objects.end()) {

        // Remove from collection
        objects.erase(it);
    }
}

/**
 *
 * Tests to see if the tester is in the collection of objects and returns
 * a boolean
 *
 * @return bool
 */
bool SmartObjectTester::contains(SmartObjectTester *value) {

    // Check if this object is contained in the collection
    return std::find(objects.begin(), objects.end(), value) != objects.end();
}

/**
 *
 * Clears the tester's object list. This should be called at the start of each
 * test case.
 *
 * @return void
 */
void SmartObjectTester::reset()
{

    // Clear collection
    objects.clear();
}

/**
 *
 * Receives some data from another object and stores it locally
 *
 * @param id The id (or type) of data
 * @param arguments The actual data
 * @return void
 */
void SmartObjectTester::processQueueItem(int id, VariantVector arguments)
{

    // Set data
    data[id] = arguments;
}

/**
 *
 * Sends data from this object, identified by id and containing arguments.
 * This is just overridden to provide public access for test cases.
 *
 * @param id The id (or type) of data
 * @param arguments The actual data
 * @return void
 */
void SmartObjectTester::queueData(int id, const VariantVector &arguments)
{
    SmartObject::queueData(id, arguments);
}

std::vector<SmartObjectTester *> SmartObjectTester::objects;

} // namespace RabidSQL

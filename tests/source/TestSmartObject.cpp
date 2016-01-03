#include "Application.h"
#include "SmartObject.h"
#include "SmartObjectTester.h"
#include "MockApplication.h"
#include "gtest/gtest.h"

using ::testing::Exactly;
using ::testing::_;

namespace RabidSQL {

class TestSmartObject : public ::testing::Test {
protected:
    void TearDown()
    {
        // Reset statically stored data
        SmartObjectTester::reset();
    }
};

TEST_F(TestSmartObject, ChildrenAutoCleanup) {
    SmartObjectTester *parent;
    SmartObjectTester *child;

    parent = new SmartObjectTester();
    child = new SmartObjectTester(parent);

    // Expect that the child's parent is set
    EXPECT_EQ(child->getParent(), parent);

    // Expect that the paren't children map to this
    EXPECT_EQ(parent->getChildren().size(), 1);

    // Expect that the first (only) child is the expected child
    EXPECT_EQ(parent->getChildren().front(), child);

    // Free memory
    delete parent;

    // Expect that the parent has freed the child
    EXPECT_FALSE(SmartObjectTester::contains(child));
}

TEST_F(TestSmartObject, ChildrenManualCleanup) {
    SmartObjectTester *parent;
    SmartObjectTester *child;

    parent = new SmartObjectTester();
    child = new SmartObjectTester(parent);

    // Free memory
    delete child;

    EXPECT_EQ(parent->getChildren().size(), 0);

    // Free memory
    delete parent;
}

// Tests that the emitter sends data to the receiver
TEST_F(TestSmartObject, SingleThreadCommunication) {
    MockApplication app;

    EXPECT_CALL(app, registerObject(_)).Times(Exactly(2));
    SmartObjectTester emitter;
    SmartObjectTester receiver;
    EXPECT_CALL(app, unregisterObject(&emitter)).Times(Exactly(1));
    EXPECT_CALL(app, unregisterObject(&receiver)).Times(Exactly(1));

    // Connect the emitter and receiver
    emitter.connectQueue(1, &receiver);

    // Send data
    emitter.queueData(1, VariantVector() << "test");

    // Process queue
    receiver.processQueue();

    EXPECT_EQ(1, receiver.data.size());
    EXPECT_EQ("test", receiver.data[1].front().toString());
}

// Tests handing arbitrary data to SmartObject
TEST_F(TestSmartObject, ArbitraryData) {
    MockApplication app;

    EXPECT_CALL(app, registerObject(_)).Times(Exactly(1));
    SmartObjectTester object;
    object.setArbitraryData("test", 123);
    EXPECT_EQ(123, object.getArbitraryData("test").toInt());
    EXPECT_TRUE(object.getArbitraryData("missing").isNull());
}

} // namespace RabidSQL

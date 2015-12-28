#include "Application.h"
#include "SmartObject.h"
#include "smartobjecttester.h"
#include "gtest/gtest.h"

namespace RabidSQL {

class TestSmartObject : public ::testing::Test {
protected:
    void TearDown()
    {
        // Reset statically stored data
        SmartObjectTester::reset();

        delete Application::getInstance();
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

// Tests that the emitter sends data to the receiever
TEST_F(TestSmartObject, SingleThreadCommunication) {
    SmartObjectTester emitter;
    SmartObjectTester receiver;

    // Connect the emitter and receiever
    emitter.connectQueue(1, &receiver);

    // Send data
    emitter.queueData(1, VariantVector() << "test");

    // Process queue
    Application::getInstance()->processEvents();

    EXPECT_EQ(receiver.data.size(), 1);
    EXPECT_EQ(receiver.data[1].front(), "test");
}

} // namespace RabidSQL



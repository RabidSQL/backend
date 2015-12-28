#include "Application.h"
#include "SmartObject.h"
#include "smartobjecttester.h"
#include "gtest/gtest.h"

namespace RabidSQL {

// Tests SmartObject auto cleanup
TEST(TestApplication, Cleanup) {

    // Reset statically stored data
    SmartObjectTester::reset();

    SmartObjectTester *object = new SmartObjectTester();

    Application::shutdown();

    // Expect that the application has freed the object and thus accessing it
    // crashes.
    EXPECT_FALSE(SmartObjectTester::contains(object));
}

// Tests message queuing
TEST(TestApplication, MessageQueues) {
    Message message;

    // Post 2 messages
    Application::postMessage(MessageType::ERROR, "err label", "err message");
    Application::postMessage(MessageType::CRITICAL, "cri label", "cri message");

    // Process first message
    message = Application::getNextMessage();
    EXPECT_EQ(MessageType::ERROR, message.type);
    EXPECT_EQ("err label", message.label);
    EXPECT_EQ(Variant("err message"), message.data);

    // Process second message
    message = Application::getNextMessage();
    EXPECT_EQ(MessageType::CRITICAL, message.type);
    EXPECT_EQ("cri label", message.label);
    EXPECT_EQ(Variant("cri message"), message.data);

    // Expect that there are 0 more messages
    EXPECT_FALSE(Application::getNextMessage());

}

} // namespace RabidSQL

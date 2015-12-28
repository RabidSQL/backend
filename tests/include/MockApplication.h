#ifndef RABIDSQL_MOCKAPPLICATION_H
#define RABIDSQL_MOCKAPPLICATION_H

#include "Application.h"

#include "gmock/gmock.h"

namespace RabidSQL {

class Application;
class MockApplication : public Application {
public:
    MOCK_METHOD3(postMessage, void(MessageType, std::string, Variant));
    MOCK_METHOD0(processEvents, void());
    MOCK_METHOD0(hasMessage, bool());
    MOCK_METHOD0(getNextMessage, Message());
    MOCK_METHOD1(registerObject, void(SmartObject *));
    MOCK_METHOD1(unregisterObject, void(SmartObject *));
};

} // namespace RabidSQL

#endif //RABIDSQL_MOCKAPPLICATION_H

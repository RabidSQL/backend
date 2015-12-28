#ifndef RABIDSQL_MOCKCONNECTIONSETTINGS_H
#define RABIDSQL_MOCKCONNECTIONSETTINGS_H

#include "ConnectionSettings.h"

#include "gmock/gmock.h"

namespace RabidSQL {

class MockConnectionSettings : public ConnectionSettings {
public:
    MockConnectionSettings() : ConnectionSettings(nullptr) {}

    MOCK_METHOD0(getParent, ConnectionSettings *());
    MOCK_METHOD1(setParent, void(ConnectionSettings *));
    MOCK_METHOD1(remove, void(std::string));
    MOCK_METHOD1(contains, bool(std::string));
    MOCK_METHOD2(get, Variant(std::string, bool));
    MOCK_METHOD2(set, void(std::string, const Variant));
    MOCK_METHOD0(getType, ConnectionType());
};

} // namespace RabidSQL

#endif //RABIDSQL_MOCKCONNECTIONSETTINGS_H

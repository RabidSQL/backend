#ifndef RABIDSQL_SETTINGSFIELD_H
#define RABIDSQL_SETTINGSFIELD_H

#include <string>

namespace RabidSQL {

struct SettingsField {
    std::string name;
    std::string label;
    std::string tooltip;
    int width = 100;
};

} // namespace RabidSQL

#endif //RABIDSQL_SETTINGSFIELD_H

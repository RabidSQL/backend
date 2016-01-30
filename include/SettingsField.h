#ifndef RABIDSQL_SETTINGSFIELD_H
#define RABIDSQL_SETTINGSFIELD_H

#include <string>
#include "NSEnums.h"
#include "Variant.h"

namespace RabidSQL {

class SettingsField {
public:
    SettingsField(std::string name, std::string label, std::string tooltip,
                  int row, DataType type = D_STRING,
                  VariantVector arguments = VariantVector());

    std::string name;
    std::string label;
    std::string tooltip;
    int row;
    DataType type;
    void *pointer;
    VariantVector arguments;
};

} // namespace RabidSQL

#endif //RABIDSQL_SETTINGSFIELD_H

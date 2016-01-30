#include "SettingsField.h"

namespace RabidSQL {

/**
 * Initializes this settings field
 */
SettingsField::SettingsField(std::string name, std::string label,
                             std::string tooltip, int row, DataType type,
                             VariantVector arguments)
{
    this->name = name;
    this->label = label;
    this->tooltip = tooltip;
    this->row = row;
    this->type = type;
    this->arguments = arguments;
    pointer = nullptr;
}

} // namespace RabidSQL
#ifndef RABIDSQL_NSENUMS_H
#define RABIDSQL_NSENUMS_H

// This wrapper exists as an easy manner to provide a namespaced enums file
// (for c++) while still allowing (but not requiring) it to exist in the global
// namespace.

#undef RABIDSQL_ENUMS_H

namespace RabidSQL {

#include "enums.h"

} // RabidSQL

#undef RABIDSQL_ENUMS_H

#endif //RABIDSQL_NSENUMS_H

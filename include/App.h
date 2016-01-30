#ifndef RABIDSQL_APP_H
#define RABIDSQL_APP_H

// Any defines to apply to everything should go here

//#define DEBUG

#ifdef DEBUG

#include "Console.h"

#define rDebug Console::debug()

#define TRACK_POINTERS
#endif

#include <assert.h>

#endif // RABIDSQL_APP_H

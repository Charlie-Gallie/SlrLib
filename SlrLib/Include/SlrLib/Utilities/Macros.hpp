#pragma once
#ifndef SLR_UTILITIES_MACROS
#define SLR_UTILITIES_MACROS

/**
* This is used when a statement is required but you want no operation to take place
* While a single semicolon can be used, this should help with clarity
*/
#define SLR_NO_OPERATION static_assert(true)

#endif // ifndef SLR_UTILITIES_MACROS
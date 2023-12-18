#pragma once
#ifndef SLR_INTERNAL_NAMESPACE
#define SLR_INTERNAL_NAMESPACE

/**
* Opens the Slr namespace
* This is used to ensure IDEs do not indent the contained code
* Use SLR_NAMESPACE_END to close the namespace
*/
#define SLR_NAMESPACE_BEGIN namespace Slr {

/**
* Closes the Slr namespace
*/
#define SLR_NAMEPSACE_END }

#endif // ifndef SLR_INTERNAL_NAMESPACE
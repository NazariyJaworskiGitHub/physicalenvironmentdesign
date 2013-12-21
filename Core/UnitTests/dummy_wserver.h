/// \file \author Nazariy Jaworski

#ifndef DUMMY_WSERVER_H
#define DUMMY_WSERVER_H

// For usage of Qt native macro SIGNAL() and SLOT()
#ifndef WT_NO_SLOT_MACROS
#define WT_NO_SLOT_MACROS
#endif // WT_NO_SLOT_MACROS

// For disabling boost warnings
#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif // BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <Wt/WServer>

// Implementation of dummy Test-case WServer (conflict between libwthttp & libwttest)
// See .cpp file

#endif // DUMMY_WSERVER_H

#ifndef PE_LIB_GLOBAL_H
#define PE_LIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PE_LIB_LIBRARY)
#  define PE_LIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PE_LIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PE_LIB_GLOBAL_H

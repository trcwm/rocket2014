#ifndef Z80EX_GLOBAL_H
#define Z80EX_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(Z80EX_LIBRARY)
#  define Z80EXSHARED_EXPORT Q_DECL_EXPORT
#else
#  define Z80EXSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // Z80EX_GLOBAL_H

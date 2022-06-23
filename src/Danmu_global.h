#ifndef DANMU_GLOBAL_H
#define DANMU_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DANMU_LIBRARY)
#  define DANMU_EXPORT Q_DECL_EXPORT
#else
#  define DANMU_EXPORT Q_DECL_IMPORT
#endif

#endif // DANMU_GLOBAL_H

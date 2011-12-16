#ifndef COMMONDEFINES_H
#define COMMONDEFINES_H

#ifdef Q_CREATOR_RUN
    #ifndef HAS_UPTODATE_QT_VERSION
        #define HAS_UPTODATE_QT_VERSION
    #endif
#elif QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    #ifndef HAS_UPTODATE_QT_VERSION
        #define HAS_UPTODATE_QT_VERSION
    #endif
#endif

#ifdef HAS_UPTODATE_QT_VERSION
    #ifndef HAS_ELAPSED_TIMER
        #define HAS_ELAPSED_TIMER
    #endif
    #ifndef HAS_VECTOR_RESERVE
       #define HAS_VECTOR_RESERVE
    #endif
    #ifndef HAS_IMAGE_CONSTSCANLINE
        #define HAS_IMAGE_CONSTSCANLINE
    #endif
#else
    #warning this qt version is outdated
#endif

#endif // COMMONDEFINES_H

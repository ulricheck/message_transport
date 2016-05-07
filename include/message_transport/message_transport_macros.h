//
// Created by Ulrich Eck on 07/05/16.
//

#ifndef MESSAGE_TRANSPORT_MESSAGE_TRANSPORT_MACROS_H
#define MESSAGE_TRANSPORT_MESSAGE_TRANSPORT_MACROS_H

#if defined(__GNUC__)
    #define MSGT_DEPRECATED __attribute__((deprecated))
    #define MSGT_FORCE_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
    #define MSGT_DEPRECATED
    #define MSGT_FORCE_INLINE __forceinline
#else
    #define MSGT_DEPRECATED
    #define MSGT_FORCE_INLINE inline
#endif


#if defined(_MSC_VER)
    #define MSGT_HELPER_IMPORT __declspec(dllimport)
    #define MSGT_HELPER_EXPORT __declspec(dllexport)
    #define MSGT_HELPER_LOCAL
#elif __GNUC__ >= 4
    #define MSGT_HELPER_IMPORT __attribute__ ((visibility("default")))
    #define MSGT_HELPER_EXPORT __attribute__ ((visibility("default")))
    #define MSGT_HELPER_LOCAL  __attribute__ ((visibility("hidden")))
#else
    #define MSGT_HELPER_IMPORT
    #define MSGT_HELPER_EXPORT
    #define MSGT_HELPER_LOCAL
#endif



#ifdef MSGT_BUILD_SHARED_LIBS // ros is being built around shared libraries
  #ifdef MSGT_EXPORTS // we are building a shared lib/dll
    #define MSGT_API_DECL MSGT_HELPER_EXPORT
  #else // we are using shared lib/dll
    #define MSGT_API_DECL MSGT_HELPER_IMPORT
  #endif
#else // ros is being built around static libraries
  #define MSGT_API_DECL
#endif

#endif //MESSAGE_TRANSPORT_MESSAGE_TRANSPORT_MACROS_H

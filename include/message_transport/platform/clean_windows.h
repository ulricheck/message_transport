//
// Created by Ulrich Eck on 11/05/16.
//

#ifndef MESSAGE_TRANSPORT_CLEAN_WINDOWS_H
#define MESSAGE_TRANSPORT_CLEAN_WINDOWS_H


// target Vista
//#ifndef WINVER
//#define WINVER 0x0601
//#endif
//
//#ifndef _WIN32_WINNT
//#define _WIN32_WINNT WINVER
//#endif
//
//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
//#define MSGT_UNDEF_WIN32_LEAN_AND_MEAN
//#endif
//
#ifndef NOMINMAX
#define NOMINMAX
#define MSGT_UNDEF_NOMINMNAX
#endif

#include <Windows.h>

#ifdef MSGT_UNDEF_NOMINMNAX
#undef NOMINMAX
#endif

//#ifdef MSGT_UNDEF_WIN32_LEAN_AND_MEAN
//#undef WIN32_LEAN_AND_MEAN
//#endif
//


#endif //MESSAGE_TRANSPORT_CLEAN_WINDOWS_H

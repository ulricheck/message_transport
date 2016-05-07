//
// Created by Ulrich Eck on 07/05/16.
//

#ifndef MESSAGE_TRANSPORT_MESSAGE_TRANSPORT_TYPES_H
#define MESSAGE_TRANSPORT_MESSAGE_TRANSPORT_TYPES_H

#if defined(_MSC_VER) && (_MSC_VER<1600) // MS express/studio 2008 or earlier
typedef          __int64  int64_t;
typedef unsigned __int64 uint64_t;
typedef          __int32  int32_t;
typedef unsigned __int32 uint32_t;
typedef          __int16  int16_t;
typedef unsigned __int16 uint16_t;
typedef          __int8    int8_t;
typedef unsigned __int8   uint8_t;
#else
#include <stdint.h>
#endif

#endif //MESSAGE_TRANSPORT_MESSAGE_TRANSPORT_TYPES_H

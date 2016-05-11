//
// Created by Ulrich Eck on 10/05/16.
//

#ifndef MESSAGE_TRANSPORT_TRACING_H
#define MESSAGE_TRANSPORT_TRACING_H

#if defined(HAVE_DTRACE) && !defined(DISABLE_DTRACE)
#include "tracing/dtrace/probes_message_transport_dtrace.h"

#define MSGT_TRACE_MESSAGE_PUBLISHED(msgid, topic) \
    if (MESSAGETRANSPORT_MESSAGE_PUBLISHED_ENABLED()) { MESSAGETRANSPORT_MESSAGE_PUBLISHED(msgid, topic); }

#define MSGT_TRACE_MESSAGE_SERIALIZED(msgid, port) \
    if (MESSAGETRANSPORT_MESSAGE_SERIALIZED_ENABLED()) { MESSAGETRANSPORT_MESSAGE_SERIALIZED(msgid, port); }

#define MSGT_TRACE_MESSAGE_RECEIVED(port) \
    if (MESSAGETRANSPORT_MESSAGE_RECEIVED_ENABLED()) { MESSAGETRANSPORT_MESSAGE_RECEIVED(port); }

#define MSGT_TRACE_MESSAGE_DESERIALIZED(msgid, port) \
    if (MESSAGETRANSPORT_MESSAGE_DESERIALIZED_ENABLED()) { MESSAGETRANSPORT_MESSAGE_DESERIALIZED(msgid, port); }

#define MSGT_TRACE_MESSAGE_CALLBACK(msgid, topic) \
    if (MESSAGETRANSPORT_MESSAGE_CALLBACK_ENABLED()) { MESSAGETRANSPORT_MESSAGE_CALLBACK(msgid, topic); }

#define MSGT_TRACING_ENABLED 1
#endif

#ifdef HAVE_ETW
//#include "tracing/etw/probes_message_transport_etw.h"
//			ETWUbitrackEventQueueDispatchEnd(m_eventDomain, messagePriority,
//											 pReceiverInfo->pPort->getComponent().getName().c_str(),
//											 pReceiverInfo->pPort->getName().c_str(),
//											 _startTime);
//#define MSGT_TRACING_ENABLED 1
#endif

#ifndef MSGT_TRACING_ENABLED
    #define MSGT_TRACE_MESSAGE_PUBLISHED(msgid, topic)
    #define MSGT_TRACE_MESSAGE_SERIALIZED(msgid, port)
    #define MSGT_TRACE_MESSAGE_RECEIVED(port)
    #define MSGT_TRACE_MESSAGE_DESERIALIZED(msgid, port)
    #define MSGT_TRACE_MESSAGE_CALLBACK(msgid, topic)
#endif


#endif //MESSAGE_TRANSPORT_TRACING_H

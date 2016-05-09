//
// Created by Ulrich Eck on 07/05/16.
//

#ifndef MESSAGE_TRANSPORT_EXCEPTIONS_H_H
#define MESSAGE_TRANSPORT_EXCEPTIONS_H_H

#include "message_transport/message_transport_macros.h"
#include <stdexcept>
#include <string>

#ifdef _MSC_VER
#pragma warning( disable : 4275 )
#endif

namespace message_transport {

class MSGT_API_DECL Exception : public std::runtime_error
{
public:
    Exception(const std::string& what)
            : std::runtime_error(what)
    {}
};

class MSGT_API_DECL StreamOverrunException : public Exception
{
public:
StreamOverrunException(const std::string& what)
        : Exception(what)
{}
};

MSGT_API_DECL void throwStreamOverrun();
}

#endif //MESSAGE_TRANSPORT_EXCEPTIONS_H_H

//
// Created by Ulrich Eck on 10/05/16.
//

#ifndef MESSAGE_TRANSPORT_INIT_LOGGING_H
#define MESSAGE_TRANSPORT_INIT_LOGGING_H


#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "message_transport/logging.h"

namespace logging = boost::log;

namespace message_transport {

void initLogging(logging::trivial::severity_level &severity) {
    logging::core::get()->set_filter
            (
                    logging::trivial::severity >= severity
            );
}
}

#endif //MESSAGE_TRANSPORT_INIT_LOGGING_H

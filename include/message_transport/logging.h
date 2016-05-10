//
// Created by Ulrich Eck on 07/05/16.
//

#ifndef MESSAGE_TRANSPORT_LOGGING_H
#define MESSAGE_TRANSPORT_LOGGING_H

#include <boost/log/trivial.hpp>

#define LOG_DEBUG(message) \
    BOOST_LOG_TRIVIAL(debug) << message

#define LOG_INFO(message) \
    BOOST_LOG_TRIVIAL(info) << message

#define LOG_WARN(message) \
    BOOST_LOG_TRIVIAL(warning) << message

#define LOG_ERROR(message) \
    BOOST_LOG_TRIVIAL(error) << message
#endif //MESSAGE_TRANSPORT_LOGGING_H

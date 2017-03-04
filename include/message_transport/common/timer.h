//
// Created by Ulrich Eck on 04/03/2017.
//

#ifndef MESSAGE_TRANSPORT_TIMER_H
#define MESSAGE_TRANSPORT_TIMER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>

#include "message_transport/message_transport_macros.h"
#include "message_transport/logging.h"

namespace message_transport {

class MSGT_API_DECL Timer {
public:
    Timer(boost::asio::io_service& io, const boost::posix_time::time_duration timeout_)
    : timer(io, timeout_)
    , timeout(timeout_)
    , flag_isRunning(false) {

    }

    /*!
     * \brief start the timer.
     */
    void start() {
        flag_isRunning = true;
        timer.async_wait(boost::bind(&Timer::callback, this));
    }

    /*!
     * \brief stop the timer.
     */
    void stop() {
        flag_isRunning = false;
    }

    /*!
     * \brief is timer running?
     */
    bool isRunning() {
        return flag_isRunning;
    }

private:

    void callback()
    {
        if (flag_isRunning) {
            try {
                timerCallback();
            }
            catch (const std::exception& e) {
                LOG_ERROR("Error during timer callback: " << e.what());
            }
            timer.expires_at(timer.expires_at() + timeout);
            timer.async_wait(boost::bind(&Timer::callback, this));
        }
    }


    virtual void timerCallback() = 0;


    boost::asio::deadline_timer timer;
    boost::posix_time::time_duration timeout;

    bool flag_isRunning;
};

} //namespace message_transport



#endif //MESSAGE_TRANSPORT_TIMER_H

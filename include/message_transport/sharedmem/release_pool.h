//
// Created by Ulrich Eck on 04/03/2017.
//

#ifndef MESSAGE_TRANSPORT_RELEASEPOOL_H
#define MESSAGE_TRANSPORT_RELEASEPOOL_H

#include <vector>
#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/range/algorithm.hpp>

#include "message_transport/message_transport_macros.h"
#include "message_transport/logging.h"
#include "message_transport/common/timer.h"

namespace mt_releasepool_anonymous {

    struct ptr_not_active {
      bool operator()(const std::vector< boost::shared_ptr<void> >::value_type& object) {
          return object.use_count()<=1;
      }
    };

}

namespace message_transport {

class MSGT_API_DECL ReleasePool: private Timer {
public:
    ReleasePool(boost::asio::io_service& io, const boost::posix_time::time_duration timeout)
    : Timer(io, timeout)
    {
        start();
    }

    ~ReleasePool() {
        stop();
    }

    template<typename T>
    void add(const boost::shared_ptr <T>& object)
    {
        if (!object)
            return;
        boost::lock_guard <boost::mutex> lock(m);
        pool.emplace_back(object);
    }
private:
    void timerCallback() override
    {
        boost::lock_guard <boost::mutex> lock(m);
        pool.erase(std::remove_if(pool.begin(), pool.end(), mt_releasepool_anonymous::ptr_not_active()),
                pool.end());
    }

    std::vector < boost::shared_ptr<void> > pool;
    boost::mutex m;
};

} //namespace message_transport

#endif //MESSAGE_TRANSPORT_RELEASEPOOL_H

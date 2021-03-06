#ifndef SHARED_MEMORY_BLOCK_DESCR_H
#define SHARED_MEMORY_BLOCK_DESCR_H

#include <cassert>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "message_transport/message_transport_macros.h"

#ifdef _MSC_VER
#pragma warning( disable : 4251 )
#endif

namespace sharedmem_transport {


    struct MSGT_API_DECL SharedMemoryBlockDescriptor
    {
        SharedMemoryBlockDescriptor();
        uint32_t getSize() const ;

        void recordSize(uint32_t newsize, uint32_t alloc=0) ;

        void allocate(const char name[256]) ;

        void reset() ;

        bool matchName(const char * name) const;

        void check_clients(boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> & lock) {
            if (num_clients) {
                //LOG_DEBUG("Waiting lock (%d clients)",num_clients);
                lockcond.wait(lock);
            }
        }

        void register_client() {
            //LOG_DEBUG("register_client:: Locking");
            boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex); 
            num_clients ++;
            //LOG_DEBUG("Registered client %d",num_clients);
        }

        void unregister_client() {
            //LOG_DEBUG("unregister_client:: Locking");
            boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex); 
            num_clients --;
            //LOG_DEBUG("Unregistered client, %d remaining",num_clients);
            assert(num_clients >= 0);
            if (num_clients == 0) {
                //LOG_DEBUG("Lock is free");
                lockcond.notify_all();
            }
            //LOG_DEBUG("unregister_client:: Unlocking");
        }

        bool wait_data_and_register_client(boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> & lock) {
            boost::posix_time::ptime max_wait = boost::posix_time::microsec_clock::universal_time()
                + boost::posix_time::seconds(1);
            //LOG_DEBUG("%d clients before wait",num_clients);
            if (!datacond.timed_wait(lock,max_wait)) {
                //LOG_DEBUG("Wait timed-out");
                return false;
            }
            //LOG_DEBUG("Wait returned %d",num_clients);
            num_clients++;
            //LOG_DEBUG("Registered client %d",num_clients);
            return true;
        }

        void signal_data() {
            datacond.notify_all();
        }

        //Mutex to protect access to the queue
        boost::interprocess::interprocess_mutex      mutex;
        boost::interprocess::interprocess_condition  lockcond;
        boost::interprocess::interprocess_condition  datacond;
        signed int num_clients;

        uint32_t size_, allocated_;

        uint32_t resize_count_;

        bool active_;
        char name_[256];
    };


}
#endif // SHARED_MEMORY_BLOCK_DESCR_H

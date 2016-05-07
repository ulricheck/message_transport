#ifndef SHAREDMEM_MESSAGE_TRANSPORT_SUBSCRIBER_H
#define SHAREDMEM_MESSAGE_TRANSPORT_SUBSCRIBER_H

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include <message_transport/common/subscriber_plugin.h>
#include <message_transport/sharedmem/SharedMemoryBlock.h>

namespace sharedmem_transport {

	template <class Base>
	class SharedmemSubscriber : public message_transport::SubscriberPlugin<Base>
	{
		public:
			SharedmemSubscriber() {
				segment_ = NULL;
                blockmgr_ = NULL;
                rec_thread_ = NULL;
			}

			virtual ~SharedmemSubscriber() {
                //LOG_DEBUG("Shutting down SharedmemSubscriber");
                if (rec_thread_) {
                    // We probably need to do something to clean up the
                    // cancelled thread here
                    rec_thread_->interrupt();
                    rec_thread_->join();
                    delete rec_thread_;
                }
                rec_thread_ = NULL;
				delete segment_;
			}

			virtual std::string getTransportName() const
			{
				return "sharedmem";
			}



		protected:
            void receiveThread() {
                //LOG_DEBUG("Receive thread running");
                while (message_transport::SubscriberPlugin<Base>::is_running) {
                    //LOG_DEBUG("Waiting for data");
                    boost::shared_ptr<Base> message_ptr(new Base);
                    if (blockmgr_->wait_data(*segment_, shm_handle_, *message_ptr) && user_cb_) {
                        (*user_cb_)(message_ptr);
                    }
                }
                //LOG_DEBUG("Unregistering client");
            }

			virtual void startReceiving(const typename message_transport::SubscriberPlugin<Base>::Callback& user_cb)
			{
				user_cb_ = &user_cb;
                //LOG_DEBUG("received latched message");
                if (!segment_) {
                    try {
                    segment_ = new boost::interprocess::managed_shared_memory(boost::interprocess::open_only,MSGTSharedMemoryDefaultBlock);
                    //LOG_DEBUG("Connected to segment");
                    } catch (boost::interprocess::bad_alloc e) {
                        segment_ = NULL;
                        //LOG_ERROR("Failed to connect to shared memory segment");
                        return;
                    }
                    blockmgr_ = (segment_->find<SharedMemoryBlock>("Manager")).first;
                    if (!blockmgr_) {
                        delete segment_;
                        segment_ = NULL;
                        //LOG_ERROR("Cannot find Manager block in shared memory segment");
                        return;
                    }
                    //LOG_DEBUG("Got block mgr %p",blockmgr_);
                    shm_handle_ = blockmgr_->findHandle(*segment_,this->getTopic().c_str());
                    if (shm_handle_.is_valid()) {
                        //LOG_DEBUG("Got shm handle %p",shm_handle_.ptr);
                        rec_thread_ = new  boost::thread(&SharedmemSubscriber::receiveThread,this);
                    } else {
                        delete segment_;
                        segment_ = NULL;
                        //LOG_ERROR("Cannot find memory block for %s", this->getTopic().c_str());
                    }
                }
			}


			const typename message_transport::SubscriberPlugin<Base>::Callback* user_cb_;
            boost::thread *rec_thread_;
			boost::interprocess::managed_shared_memory *segment_ ;
            SharedMemoryBlock *blockmgr_;
			shm_handle shm_handle_;
	};

} //namespace transport

#endif // SHAREDMEM_MESSAGE_TRANSPORT_SUBSCRIBER_H

#ifndef SHAREDMEM_TRANSPORT_PUBLISHER_H
#define SHAREDMEM_TRANSPORT_PUBLISHER_H

#include "message_transport/message_transport_macros.h"
#include "message_transport/common/publisher_plugin.h"
#include "message_transport/serialization/serialization.h"
#include "message_transport/sharedmem/SharedMemoryBlock.h"
#include "message_transport/logging.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

namespace sharedmem_transport {
namespace pt = boost::property_tree;

	class SharedmemPublisherImpl 
	{
		public:
			SharedmemPublisherImpl(const boost::shared_ptr< pt::ptree >& config);
			virtual ~SharedmemPublisherImpl();

			uint32_t initialise(const std::string & topic);

            template <class M>
			void publish_msg(const M& message) {
				// @todo Find a way to compute the (approximate) size of a serialized object (part of serialization framework)
                uint32_t serlen = message_transport::serialization::serializationLength(message);

                if (!shm_handle_.is_valid()) {
                    LOG_DEBUG("Ignoring publish request on an invalid handle");
                    return;
                }
                blockmgr_->reallocateBlock(*segment_,shm_handle_,serlen);
                if (shm_handle_.is_valid()) { // check again, in case reallocate failed
                    blockmgr_->serialize(*segment_,shm_handle_,message);
                }
            }
		protected:
			boost::interprocess::managed_shared_memory *segment_ ;
            SharedMemoryBlock *blockmgr_;
			bool clientRegistered;

			// This will be modified after the first image is received, so we
			// mark them mutable and publish stays "const"
            shm_handle shm_handle_;

            boost::shared_ptr< pt::ptree > config_;

	};

	template <class Base>
	class MSGT_API_DECL SharedmemPublisher : 
		public message_transport::PublisherPlugin<Base>
	{
		public:
			SharedmemPublisher(const boost::shared_ptr< pt::ptree >& config, const std::string& base_topic) :
                message_transport::PublisherPlugin<Base>(config, base_topic),
                impl(config),
                first_run_(true) {}

			virtual ~SharedmemPublisher() {}

			virtual std::string getTransportName() const
			{
				return "sharedmem";
			}

			virtual uint32_t getNumSubscribers() {
				// @todo future work: extend sharedmem_transport to support subscriber counting ..
				return 1;
			};

		virtual void shutdown() {
			// some shutdown action needed ?
		}

		protected:

			virtual void publish(const Base& message) const {
                LOG_DEBUG("Publishing shm message");
				if (first_run_) {
					LOG_INFO("First publish run");
					uint32_t handle = impl.initialise(this->getTopic());
					// @todo should publish the availability of a message-topic on a shm-message-queue
					LOG_INFO("Publishing latched header");
					first_run_ = false;
				}
                impl.publish_msg(message);
			}

			mutable SharedmemPublisherImpl impl;
            mutable bool first_run_;

	};

} //namespace sharedmem_transport


#endif // SHAREDMEM_TRANSPORT_PUBLISHER_H

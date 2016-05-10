#ifndef MESSAGE_TRANSPORT_PUBLISHER_INL_H
#define MESSAGE_TRANSPORT_PUBLISHER_INL_H

#include "message_transport/common/publisher.h"
#include "message_transport/common/publisher_impl.h"
#include "message_transport/serialization/message_types.h"
#include "message_transport/sharedmem/sharedmem_publisher.h"

namespace message_transport {
namespace pt = boost::property_tree;

	template <class M>
	void Publisher::publish(const M& message) const
	{
		if (!impl_ || !impl_->isValid()) {
			//ROS_ASSERT_MSG(false, "Call to publish() on an invalid message_transport::Publisher");
			return;
		}

		impl_->publish<M>(message);
	}

	/*!
	 * \brief Publish an image on the topics associated with this Publisher.
	 */
	template <class M>
	void Publisher::publish(const typename M::ConstPtr& message) const
	{
		if (!impl_ || !impl_->isValid()) {
			//ROS_ASSERT_MSG(false, "Call to publish() on an invalid message_transport::Publisher");
			return;
		}

		impl_->publish<M>(message);
	}

	bool Publisher::operator< (const Publisher& rhs) const { return impl_ <  rhs.impl_; }
	bool Publisher::operator!=(const Publisher& rhs) const { return impl_ != rhs.impl_; }
	bool Publisher::operator==(const Publisher& rhs) const { return impl_ == rhs.impl_; }


	template <class M>
	void Publisher::do_initialise(const std::string& base_topic)
	{
		assert(impl_ == NULL);
		PublisherImpl<M>* impl = new PublisherImpl<M>(config_, base_topic);
		impl_.reset(impl);

		BOOST_FOREACH(const std::string& lookup_name, impl->getDeclaredClasses()) {
			try {
				boost::shared_ptr< PublisherPlugin<M> > pub = impl->addInstance(lookup_name);
                // maybe initialize an instance ??
			}
			catch (const std::runtime_error& e) {
				//LOG_DEBUG("Failed to load plugin %s, error string: %s", lookup_name.c_str(), e.what());
			}
		}

		if (impl->getNumPublishers() == 0) {
			throw std::runtime_error("No plugins found!");
		}
	}


// instantiate templated functions
template MSGT_API_DECL void Publisher::publish<serialization::RawMessage>(const serialization::RawMessage& message) const;
template MSGT_API_DECL void Publisher::publish<serialization::RawMessage>(const serialization::RawMessage::ConstPtr& message) const;
template MSGT_API_DECL void Publisher::do_initialise<serialization::RawMessage>(const std::string& base_topic);

} //namespace message_transport

#endif

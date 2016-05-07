
#ifndef MESSAGE_TRANSPORT_SUBSCRIBER_H
#define MESSAGE_TRANSPORT_SUBSCRIBER_H

#include "message_transport/common/subscriber_impl.h"

namespace message_transport {

	/**
	 * \brief Manages a subscription callback on a specific topic that can be interpreted
	 * as an Message topic.
	 *
	 * Subscriber is the client-side counterpart to Publisher. By loading the
	 * appropriate plugin, it can subscribe to a base image topic using any available
	 * transport. The complexity of what transport is actually used is hidden from the user,
	 * who sees only a normal Message callback.
	 *
	 * A Subscriber should always be created through a call to MessageTransport::subscribe(),
	 * or copied from one that was.
	 * Once all copies of a specific Subscriber go out of scope, the subscription callback
	 * associated with that handle will stop being called. Once all Subscriber for a given
	 * topic go out of scope the topic will be unsubscribed.
	 */
	class Subscriber
	{
		public:
			// need to pass topic here ??
			Subscriber();

			std::string getTopic() const;

			/**
			 * \brief Returns the number of publishers this subscriber is connected to.
			 */
			uint32_t getNumPublishers() const;

			/**
			 * \brief Unsubscribe the callback associated with this Subscriber.
			 */
			void shutdown();

			operator void*() const;
			bool operator< (const Subscriber& rhs) const { return impl_ <  rhs.impl_; }
			bool operator!=(const Subscriber& rhs) const { return impl_ != rhs.impl_; }
			bool operator==(const Subscriber& rhs) const { return impl_ == rhs.impl_; }

			template <class M>
			int do_subscribe(const std::string& base_topic, const std::string& transport_name, uint32_t queue_size,
					const boost::function<void(const typename M::ConstPtr&)>& callback)
			{
				// Tell plugin to subscribe.
				impl_.reset(new SubscriberImpl<M>());
				// Load the plugin for the chosen transport.
				impl_.get()->reset(transport_name);

				impl_->getTemplateSubscriber<M>()->subscribe(base_topic, queue_size, callback);
				return 0;
			}

			// @todo implement do_unsubscribe

		private:

			typedef boost::shared_ptr<SubscriberImplGen> ImplPtr;

			ImplPtr impl_;

	};

} //namespace message_transport

#endif

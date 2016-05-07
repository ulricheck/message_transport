#ifndef MESSAGE_TRANSPORT_PUBLISHER_H
#define MESSAGE_TRANSPORT_PUBLISHER_H

#include "message_transport/common/publisher_impl.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

namespace message_transport {

	/**
	 * \brief Manages advertisements of multiple transport options on an Message topic.
	 *
	 * Publisher is a drop-in replacement for ros::Publisher when publishing
	 * Message topics. In a minimally built environment, they behave the same; however,
	 * Publisher is extensible via plugins to publish alternative representations of
	 * the image on related subtopics. This is especially useful for limiting bandwidth and
	 * latency over a network connection, when you might (for example) use the theora plugin
	 * to transport the images as streamed video. All topics are published only on demand
	 * (i.e. if there are subscribers).
	 *
	 * A Publisher should always be created through a call to MessageTransport::advertise(),
	 * or copied from one that was.
	 * Once all copies of a specific Publisher go out of scope, any subscriber callbacks
	 * associated with that handle will stop being called. Once all Publisher for a
	 * given base topic go out of scope the topic (and all subtopics) will be unadvertised.
	 */
	class Publisher
	{
		public:
//			Publisher() {}
			Publisher();

			/*!
			 * \brief Returns the number of subscribers that are currently connected to
			 * this Publisher.
			 *
			 * Returns the total number of subscribers to all advertised topics.
			 */
			uint32_t getNumSubscribers() const;

			/*!
			 * \brief Returns the base topic of this Publisher.
			 */
			std::string getTopic() const;

			/*!
			 * \brief Publish an image on the topics associated with this Publisher.
			 */
			template <class M>
			void publish(const M& message) const
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
			void publish(const typename M::ConstPtr& message) const
			{
				if (!impl_ || !impl_->isValid()) {
					//ROS_ASSERT_MSG(false, "Call to publish() on an invalid message_transport::Publisher");
					return;
				}

				impl_->publish<M>(message);
			}

			/*!
			 * \brief Shutdown the advertisements associated with this Publisher.
			 */
			void shutdown();

			operator void*() const;
			bool operator< (const Publisher& rhs) const { return impl_ <  rhs.impl_; }
			bool operator!=(const Publisher& rhs) const { return impl_ != rhs.impl_; }
			bool operator==(const Publisher& rhs) const { return impl_ == rhs.impl_; }


			template <class M>
			void do_initialise(const std::string& base_topic)
			{
				assert(impl_ == NULL);
				PublisherImpl<M>* impl = new PublisherImpl<M>(base_topic);
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

		private:

			typedef boost::shared_ptr<PublisherImplGen> ImplPtr;

			ImplPtr impl_;

	};

} //namespace message_transport

#endif

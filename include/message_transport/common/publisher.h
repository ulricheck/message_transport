#ifndef MESSAGE_TRANSPORT_PUBLISHER_H
#define MESSAGE_TRANSPORT_PUBLISHER_H

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

#include "message_transport/message_transport_macros.h"
#include "message_transport/message_transport_types.h"

namespace message_transport {
namespace pt = boost::property_tree;


	// forward declarations
	class PublisherImplGen;

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
	class MSGT_API_DECL Publisher
	{
		public:
			Publisher(const boost::shared_ptr< pt::ptree >& config) : config_(config) {}
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
			void publish(const M& message) const;

			/*!
			 * \brief Publish an image on the topics associated with this Publisher.
			 */
			template <class M>
			void publish(const typename M::ConstPtr& message) const;

			/*!
			 * \brief Shutdown the advertisements associated with this Publisher.
			 */
			void shutdown();

			operator void*() const;
			bool operator< (const Publisher& rhs) const;
			bool operator!=(const Publisher& rhs) const;
			bool operator==(const Publisher& rhs) const;


			template <class M>
			void do_initialise(const std::string& base_topic);

		private:

			typedef boost::shared_ptr<PublisherImplGen> ImplPtr;

			ImplPtr impl_;
			boost::shared_ptr< pt::ptree > config_;

	};

} //namespace message_transport

#endif

#ifndef MESSAGE_TRANSPORT_PUBLISHER_PLUGIN_H
#define MESSAGE_TRANSPORT_PUBLISHER_PLUGIN_H

#include <string>
#include <boost/noncopyable.hpp>

namespace message_transport {

	/**
	 * \brief Base class for plugins to Publisher.
	 */
	class PublisherPluginGen : boost::noncopyable
	{
		public:
			virtual ~PublisherPluginGen() {}

			/**
			 * \brief Get a string identifier for the transport provided by
			 * this plugin.
			 */
			virtual std::string getTransportName() const = 0;

			/**
			 * \brief Returns the number of subscribers that are currently connected to
			 * this PublisherPlugin.
			 */
			virtual uint32_t getNumSubscribers() const = 0;

			/**
			 * \brief Returns the communication topic that this PublisherPlugin will publish on.
			 */
			virtual std::string getTopic() const = 0;

			/**
			 * \brief Shutdown any advertisements associated with this PublisherPlugin.
			 */
			virtual void shutdown() = 0;

			/**
			 * \brief Return the lookup name of the PublisherPlugin associated with a specific
			 * transport identifier.
			 */
			static std::string getLookupName(const std::string& transport_name)
			{
				return transport_name + "_pub";
			}

		protected:
	};

	/**
	 * \brief Base class for plugins to Publisher.
	 */
	template <class M>
	class PublisherPlugin : public PublisherPluginGen
	{
		public:
			virtual ~PublisherPlugin() {}

			/**
			 * \brief Publish an image using the transport associated with this PublisherPlugin.
			 */
			virtual void publish(const M& message) const = 0;

			/**
			 * \brief Publish an image using the transport associated with this PublisherPlugin.
			 */
			virtual void publish(const typename M::ConstPtr& message) const
			{
				publish(*message);
			}

		protected:

	};

} //namespace message_transport

#endif

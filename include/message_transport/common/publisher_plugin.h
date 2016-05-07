#ifndef MESSAGE_TRANSPORT_PUBLISHER_PLUGIN_H
#define MESSAGE_TRANSPORT_PUBLISHER_PLUGIN_H

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

namespace message_transport {
namespace pt = boost::property_tree;

	/**
	 * \brief Base class for plugins to Publisher.
	 */
	class PublisherPluginGen : boost::noncopyable
	{
		public:
			PublisherPluginGen(const boost::shared_ptr< pt::ptree >& config) : config_(config) {}
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
			boost::shared_ptr< pt::ptree > config_;	};

	/**
	 * \brief Base class for plugins to Publisher.
	 */
	template <class M>
	class PublisherPlugin : public PublisherPluginGen
	{
		public:
			PublisherPlugin(const boost::shared_ptr< pt::ptree >& config, const std::string& base_topic)
                    : PublisherPluginGen(config), base_topic_(base_topic) {}
			virtual ~PublisherPlugin() {}


            virtual uint32_t getNumSubscribers() const {
                // @todo needs infrastructure for subscriber counting
                return 1;
            }

            virtual std::string getTopic() const {
                return base_topic_;
            }

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
            std::string base_topic_;

	};

} //namespace message_transport

#endif

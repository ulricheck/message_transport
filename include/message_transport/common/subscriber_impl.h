#ifndef MESSAGE_TRANSPORT_SUBSCRIBER_IMPL_H
#define MESSAGE_TRANSPORT_SUBSCRIBER_IMPL_H


#include <vector>
#include "message_transport/common/subscriber_plugin.h"
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

namespace message_transport {
namespace pt = boost::property_tree;

	class  SubscriberImplGen
	{
		public :
			SubscriberImplGen(const boost::shared_ptr< pt::ptree >& config) : unsubscribed_(false), config_(config)  { }

			~SubscriberImplGen()
			{
			}

			bool isValid() const
			{
				return !unsubscribed_;
			}

			virtual void reset(const std::string& transport_name) = 0;
			void shutdown() {
				this->shutdownImpl();
			}
			virtual void shutdownImpl() = 0;
			virtual boost::shared_ptr< SubscriberPluginGen > getSubscriber() = 0;

			template <class M> 
            boost::shared_ptr< SubscriberPlugin<M> > getTemplateSubscriber() {
                return boost::dynamic_pointer_cast< SubscriberPlugin<M> >(getSubscriber());
            }


			virtual std::vector<std::string> getDeclaredClasses() = 0;
		protected :

			bool unsubscribed_;
			boost::shared_ptr< pt::ptree > config_;
	};

	template <class M>
    class  SubscriberImpl : public SubscriberImplGen
	{
		public:
			SubscriberImpl(const boost::shared_ptr< pt::ptree >& config) : SubscriberImplGen(config) { }

			~SubscriberImpl() {
				shutdownImpl();
			}

			virtual void shutdownImpl()
			{
				if (!unsubscribed_) {
					unsubscribed_ = true;
					subscriber_->shutdown();
				}
			}

			virtual void reset(const std::string& transport_name);

			virtual boost::shared_ptr< SubscriberPluginGen > getSubscriber() {
				return subscriber_;
			}

			virtual std::vector<std::string> getDeclaredClasses();

		protected:

            boost::shared_ptr< SubscriberPlugin<M> > subscriber_;
	};
};

#endif // MESSAGE_TRANSPORT_SUBSCRIBER_IMPL_H

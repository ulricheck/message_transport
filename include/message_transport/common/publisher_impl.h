#ifndef MESSAGE_TRANSPORT_PUBLISHER_IMPL_H
#define MESSAGE_TRANSPORT_PUBLISHER_IMPL_H

#include <string>
#include <vector>

#include "message_transport/common/publisher_plugin.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/property_tree/ptree.hpp>

namespace message_transport {
namespace pt = boost::property_tree;

	class PublisherImplGen
	{
		public:
			PublisherImplGen(const boost::shared_ptr< pt::ptree >& config, const std::string & topic) :
				config_(config), base_topic_(topic), unsubscribed_(false) { }

			virtual ~PublisherImplGen()
			{
			}

			virtual uint32_t getNumSubscribers() const = 0;

			uint32_t getNumSubscribersBindable() const {
				return this->getNumSubscribers();
			}

            virtual bool isValid() {
                // maybe some initialization will be needed in the future ..
                return !unsubscribed_;
            }

			std::string getTopic() const
			{
				return base_topic_;
			}

			void shutdown() {
				this->shutdownImpl();
			}

			virtual void shutdownImpl() = 0;

			virtual std::vector<std::string> getDeclaredClasses() = 0;

			template <class M>
            void publish(const M& message) const;

			template <class M>
            void publish(const typename M::ConstPtr& message) const;

		protected :
			std::string base_topic_;
            bool unsubscribed_;
            boost::shared_ptr< pt::ptree > config_;
	};

	template <class M>
	class PublisherImpl : public PublisherImplGen
	{
		public :
			PublisherImpl(const boost::shared_ptr< pt::ptree >& config, const std::string & topic)
                    : PublisherImplGen(config, topic)
		{
		}

			virtual ~PublisherImpl() {
				shutdownImpl();
			}

			virtual uint32_t getNumSubscribers() const
			{
				uint32_t count = 0;
                for (unsigned int i=0;i<publishers_.size();i++) {
					count += publishers_[i]->getNumSubscribers();
                }
				return count;
			}

			virtual void shutdownImpl()
			{
                if (!unsubscribed_){
                    for (unsigned int i=0;i<publishers_.size();i++) {
                        publishers_[i]->shutdown();
                    }
                    publishers_.clear();
                }
			}

			virtual std::vector<std::string> getDeclaredClasses();

            boost::shared_ptr< PublisherPlugin<M> > addInstance(const std::string & name);

			uint32_t getNumPublishers() const {
				return publishers_.size();
			}

			void publish(const M& message) const {
                for (unsigned int i=0;i<publishers_.size();i++) {
					if (publishers_[i]->getNumSubscribers() > 0) {
						publishers_[i]->publish(message);
					}
				}
			}

			void publish(const typename M::ConstPtr& message) const {
                for (unsigned int i=0;i<publishers_.size();i++) {
					if (publishers_[i]->getNumSubscribers() > 0) {
						publishers_[i]->publish(message);
					}
				}
			}
		protected:
            std::vector<boost::shared_ptr< PublisherPlugin<M> > > publishers_;
	};

	template <class M>
    void PublisherImplGen::publish(const M& message) const
    {
        (dynamic_cast<const PublisherImpl<M>* const>(this))->publish(message);
    }

	template <class M>
    void PublisherImplGen::publish(const typename M::ConstPtr& message) const
    {
        (dynamic_cast<const PublisherImpl<M>* const>(this))->publish(message);
    }
};
#endif // MESSAGE_TRANSPORT_PUBLISHER_IMPL_H

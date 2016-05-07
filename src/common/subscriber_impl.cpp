//
// Created by Ulrich Eck on 07/05/16.
//

#include "message_transport/common/subscriber_impl.h"
#include "message_transport/sharedmem/sharedmem_subscriber.h"

namespace message_transport {

std::vector<std::string> SubscriberImpl::getDeclaredClasses()
{
    std::vector<std::string> result;
    result.push_back("sharedmem_sub");
    return result;
}

template< class M>
void SubscriberImpl::reset(const std::string& transport_name)
{
    std::string lookup_name = SubscriberPluginGen::getLookupName(transport_name);
    if (lookup_name.compare("sharedmem_sub") == 0) {
        subscriber_.reset(new sharedmem_transport::SharedmemSubscriber<M>());
    } else {
        throw std::runtime_error("Invalid transport");
    }
}

}
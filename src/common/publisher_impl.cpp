//
// Created by Ulrich Eck on 07/05/16.
//

#include "message_transport/common/publisher_impl.h"
#include "message_transport/sharedmem/sharedmem_publisher.h"

namespace message_transport {

std::vector<std::string> PublisherImpl::getDeclaredClasses()
{
    std::vector<std::string> result;
    result.push_back("sharedmem_pub");
    return result;
}

template <class M>
boost::shared_ptr<PublisherPlugin<M> > PublisherImpl::addInstance(const std::string& name)
{
    boost::shared_ptr<PublisherPlugin<M> > pub; // = loader_.createInstance(name);
    if (name.compare("sharedmem_pub") == 0) {
        pub.reset(new sharedmem_transport::SharedmemPublisher<M>());
    } else {
        throw std::runtime_error("Invalid transport");
    }
    publishers_.push_back(pub);
    return pub;
}

}
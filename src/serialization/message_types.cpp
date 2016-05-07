//
// Created by Ulrich Eck on 07/05/16.
//

#include "message_transport/sharedmem/sharedmem_publisher.h"
#include "message_transport/sharedmem/sharedmem_subscriber.h"
#include "message_transport/common/publisher_impl.h"
#include "message_transport/common/subscriber_impl.h"

#include "message_transport/serialization/message_types.h"


template class sharedmem_transport::SharedmemSubscriber<message_transport::serialization::TestMessage>;
template class sharedmem_transport::SharedmemPublisher<message_transport::serialization::TestMessage>;
template class message_transport::SubscriberImpl<message_transport::serialization::TestMessage>;
template class message_transport::PublisherImpl<message_transport::serialization::TestMessage>;
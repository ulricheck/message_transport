
#include "message_transport/common/subscriber.h"
#include "message_transport/common/subscriber_plugin.h"
#include <boost/scoped_ptr.hpp>

namespace message_transport {

Subscriber::Subscriber()
  : impl_()
{

}

std::string Subscriber::getTopic() const
{
  if (impl_) return impl_->getSubscriber()->getTopic();
  return std::string();
}

uint32_t Subscriber::getNumPublishers() const
{
  if (impl_) return impl_->getSubscriber()->getNumPublishers();
  return 0;
}

void Subscriber::shutdown()
{
  if (impl_) impl_->shutdown();
}

Subscriber::operator void*() const
{
  return (impl_ && impl_->isValid()) ? (void*)1 : (void*)0;

} //namespace message_transport

}
#include "message_transport/common/publisher.inl"
#include "message_transport/common/publisher_plugin.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/erase.hpp>

namespace message_transport {

Publisher::Publisher() : impl_() {
}

uint32_t Publisher::getNumSubscribers() const
{
  if (impl_ && impl_->isValid()) return impl_->getNumSubscribers();
  return 0;
}

std::string Publisher::getTopic() const
{
  if (impl_) return impl_->getTopic();
  return std::string();
}


void Publisher::shutdown()
{
  if (impl_) {
    impl_->shutdown();
    impl_.reset();
  }
}

Publisher::operator void*() const
{
  return (impl_ && impl_->isValid()) ? (void*)1 : (void*)0;
}


} //namespace message_transport

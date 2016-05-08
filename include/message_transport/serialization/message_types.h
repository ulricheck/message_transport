//
// Created by Ulrich Eck on 07/05/16.
//

#ifndef MESSAGE_TRANSPORT_MESSAGE_TYPES_H
#define MESSAGE_TRANSPORT_MESSAGE_TYPES_H


#include <string>
#include <boost/shared_ptr.hpp>
#include "message_transport/serialization/serialization.h"

namespace message_transport {
namespace serialization {

// for now only testing ..

template <class ContainerAllocator>
class TestMessage_ {
public:

    typedef TestMessage_<ContainerAllocator> Type;
    typedef boost::shared_ptr< TestMessage_<ContainerAllocator> > Ptr;
    typedef boost::shared_ptr< TestMessage_<ContainerAllocator>  const> ConstPtr;


    TestMessage_() : value(0) {}
    TestMessage_(const ContainerAllocator& _alloc) : value(0) {}


    int value;
};
typedef TestMessage_<std::allocator<void> > TestMessage;

typedef boost::shared_ptr< TestMessage> TestMessagePtr;
typedef boost::shared_ptr< TestMessage const> TestMessageConstPtr;



/**
 * \brief Serializer specialized for TestMessage
 */
template<>
struct Serializer<TestMessage>
{
  template<typename Stream>
  inline static void write(Stream& stream, const TestMessage& v)
  {
      stream.next(v.value);
  }

  template<typename Stream>
  inline static void read(Stream& stream, TestMessage& v)
  {
      stream.next(v.value);
  }

  inline static uint32_t serializedLength(const TestMessage&)
  {
      return 4;
  }
};

}
}

#endif //MESSAGE_TRANSPORT_MESSAGE_TYPES_H

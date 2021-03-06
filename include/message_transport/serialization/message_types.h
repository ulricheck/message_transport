//
// Created by Ulrich Eck on 07/05/16.
//

#ifndef MESSAGE_TRANSPORT_MESSAGE_TYPES_H
#define MESSAGE_TRANSPORT_MESSAGE_TYPES_H


#include <string>
#include <memory>

#include <boost/shared_ptr.hpp>
#include "message_transport/serialization/serialization.h"
#include "message_transport/message_transport_traits.h"
#include "message_transport/message_transport_util.h"

namespace message_transport {
namespace serialization {

// for now only testing ..

template <class ContainerAllocator>
class RawMessage_ {
public:

    typedef RawMessage_<ContainerAllocator> Type;
    typedef boost::shared_ptr< RawMessage_<ContainerAllocator> > Ptr;
    typedef boost::shared_ptr< RawMessage_<ContainerAllocator>  const> ConstPtr;


    RawMessage_() : m_value(nullptr), m_allocated_bytes(0), timestamp(0) {}
    RawMessage_(const ContainerAllocator& _alloc) : m_value(nullptr), m_allocated_bytes(0), timestamp(0) {}

    bool allocate(uint32_t size) {
        if (size == m_allocated_bytes) {
            return true;
        }

        ContainerAllocator alloc;
        if (m_allocated_bytes > 0) {
            alloc.deallocate(m_value, m_allocated_bytes);
            m_allocated_bytes = 0;
        }
        if (size > 0) {
            try {
                m_value = alloc.allocate(size);
                m_allocated_bytes = size;
            } catch (std::bad_alloc &) {
                return false;
            }
        }
        return true;
    }

    uint8_t* getDataPtr() {
        if (m_allocated_bytes == 0) {
            return nullptr;
        }
        return m_value;
    }

    uint8_t* getDataConstPtr() const {
        if (m_allocated_bytes == 0) {
            return nullptr;
        }
        return m_value;
    }

    uint32_t getDataLength() const {
        return m_allocated_bytes;
    }

    message_transport::Timestamp timestamp;

protected:
    uint8_t* m_value;
    uint32_t m_allocated_bytes;
};
typedef RawMessage_<std::allocator<uint8_t> > RawMessage;

typedef boost::shared_ptr< RawMessage> RawMessagePtr;
typedef boost::shared_ptr< RawMessage const> RawMessageConstPtr;



/**
 * \brief Serializer specialized for RawMessage
 * @todo is it possible to avoid copying here ?
 */
template<>
struct Serializer<RawMessage>
{
  template<typename Stream>
  inline static void write(Stream& stream, const RawMessage& v)
  {
      stream.next(v.timestamp);
      uint32_t data_size = v.getDataLength();
      if (data_size > 0) {
          memcpy(stream.advance(data_size), v.getDataConstPtr(), data_size);
      }
  }

  template<typename Stream>
  inline static void read(Stream& stream, RawMessage& v)
  {
      stream.next(v.timestamp);
      uint32_t data_size = stream.getLength();
      if (v.allocate(data_size)) {
          if (data_size > 0) {
              memcpy(v.getDataPtr(), stream.advance(data_size), data_size);
          }
      }
  }

  inline static uint32_t serializedLength(const RawMessage& v)
  {
      return sizeof(Timestamp) + v.getDataLength();
  }
};

}
namespace message_traits {

template<>
struct MessageID<serialization::RawMessage> {
  static const unsigned long long getMessageID(const serialization::RawMessage& message) { return message.timestamp; }
};

}
}

#endif //MESSAGE_TRANSPORT_MESSAGE_TYPES_H

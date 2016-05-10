//
// Created by Ulrich Eck on 07/05/16.
//

#ifndef MESSAGE_TRANSPORT_MESSAGE_TYPES_H
#define MESSAGE_TRANSPORT_MESSAGE_TYPES_H


#include <string>
#include <memory>

#include <boost/shared_ptr.hpp>
#include "message_transport/serialization/serialization.h"

namespace message_transport {
namespace serialization {

// for now only testing ..

template <class ContainerAllocator>
class RawMessage_ {
public:

    typedef RawMessage_<ContainerAllocator> Type;
    typedef boost::shared_ptr< RawMessage_<ContainerAllocator> > Ptr;
    typedef boost::shared_ptr< RawMessage_<ContainerAllocator>  const> ConstPtr;


    RawMessage_() : value(nullptr), allocated_bytes(0) {}
    RawMessage_(const ContainerAllocator& _alloc) : value(nullptr), allocated_bytes(0) {}

    bool allocate(uint32_t size) {
        if (size == allocated_bytes) {
            return true;
        }

        ContainerAllocator alloc;
        if (allocated_bytes > 0) {
            alloc.deallocate(value, allocated_bytes);
            allocated_bytes = 0;
        }
        if (size > 0) {
            try {
                value = alloc.allocate(size);
                allocated_bytes = size;
            } catch (std::bad_alloc &) {
                return false;
            }
        }
        return true;
    }

    uint8_t* getDataPtr() {
        if (allocated_bytes == 0) {
            return nullptr;
        }
        return value;
    }

    uint8_t* getDataConstPtr() const {
        if (allocated_bytes == 0) {
            return nullptr;
        }
        return value;
    }

    uint32_t getLength() const {
        return allocated_bytes;
    }

protected:
    uint8_t* value;
    uint32_t allocated_bytes;
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
      uint32_t data_size = v.getLength();
      if (data_size > 0) {
          memcpy(stream.advance(data_size), v.getDataConstPtr(), data_size);
      }
  }

  template<typename Stream>
  inline static void read(Stream& stream, RawMessage& v)
  {
      uint32_t data_size = stream.getLength();
      if (v.allocate(data_size)) {
          if (data_size > 0) {
              memcpy(v.getDataPtr(), stream.advance(data_size), data_size);
          }
      }
  }

  inline static uint32_t serializedLength(const RawMessage& v)
  {
      return v.getLength();
  }
};

}
}

#endif //MESSAGE_TRANSPORT_MESSAGE_TYPES_H

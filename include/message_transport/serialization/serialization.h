//
// Created by Ulrich Eck on 07/05/16.
//

#ifndef MESSAGE_TRANSPORT_SERIALIZATION_H
#define MESSAGE_TRANSPORT_SERIALIZATION_H


#include <boost/array.hpp>
#include <boost/call_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>



/**
 * \brief Templated serialization class.  Default implementation provides backwards compatibility with
 * old message types.
 *
 * Specializing the Serializer class is the only thing you need to do to get the ROS serialization system
 * to work with a type.
 */
template<typename T>
struct Serializer
{
  /**
   * \brief Write an object to the stream.  Normally the stream passed in here will be a ros::serialization::OStream
   */
  template<typename Stream>
  inline static void write(Stream& stream, typename boost::call_traits<T>::param_type t)
  {
      t.serialize(stream.getData(), 0);
  }

  /**
   * \brief Read an object from the stream.  Normally the stream passed in here will be a ros::serialization::IStream
   */
  template<typename Stream>
  inline static void read(Stream& stream, typename boost::call_traits<T>::reference t)
  {
      t.deserialize(stream.getData());
  }

  /**
   * \brief Determine the serialized length of an object.
   */
  inline static uint32_t serializedLength(typename boost::call_traits<T>::param_type t)
  {
      return t.serializationLength();
  }
};


/**
 * \brief Serialize an object.  Stream here should normally be a ros::serialization::OStream
 */
template<typename T, typename Stream>
inline void serialize(Stream& stream, const T& t)
{
    Serializer<T>::write(stream, t);
}

/**
 * \brief Deserialize an object.  Stream here should normally be a ros::serialization::IStream
 */
template<typename T, typename Stream>
inline void deserialize(Stream& stream, T& t)
{
    Serializer<T>::read(stream, t);
}

/**
 * \brief Determine the serialized length of an object
 */
template<typename T>
inline uint32_t serializationLength(const T& t)
{
    return Serializer<T>::serializedLength(t);
}


#endif //MESSAGE_TRANSPORT_SERIALIZATION_H

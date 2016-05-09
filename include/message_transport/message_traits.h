//
// Created by Ulrich Eck on 07/05/16.
//

#ifndef MESSAGE_TRANSPORT_MESSAGE_TRAITS_H
#define MESSAGE_TRANSPORT_MESSAGE_TRAITS_H

#include <string>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include "message_transport/message_transport_types.h"

namespace message_transport {
namespace message_traits {

/**
 * \brief Base type for compile-type true/false tests.  Compatible with Boost.MPL.  classes inheriting from this type
 * are \b true values.
 */
struct TrueType
{
  static const bool value = true;
  typedef TrueType type;
};

/**
 * \brief Base type for compile-type true/false tests.  Compatible with Boost.MPL.  classes inheriting from this type
 * are \b false values.
 */
struct FalseType
{
  static const bool value = false;
  typedef FalseType type;
};

/**
 * \brief A simple datatype is one that can be memcpy'd directly in array form, i.e. it's a POD, fixed-size type and
 * sizeof(M) == sum(serializationLength(M:a...))
 */
template<typename M> struct IsSimple : public FalseType {};
/**
 * \brief A fixed-size datatype is one whose size is constant, i.e. it has no variable-length arrays or strings
 */
template<typename M> struct IsFixedSize : public FalseType {};

/**
 * \brief returns IsSimple<M>::value;
 */
template<typename M>
inline bool isSimple()
{
    return IsSimple<typename boost::remove_reference<typename boost::remove_const<M>::type>::type>::value;
}

/**
 * \brief returns IsFixedSize<M>::value;
 */
template<typename M>
inline bool isFixedSize()
{
    return IsFixedSize<typename boost::remove_reference<typename boost::remove_const<M>::type>::type>::value;
}

#define MSGT_CREATE_SIMPLE_TRAITS(Type) \
    template<> struct IsSimple<Type> : public TrueType {}; \
    template<> struct IsFixedSize<Type> : public TrueType {};

MSGT_CREATE_SIMPLE_TRAITS(uint8_t);
MSGT_CREATE_SIMPLE_TRAITS(int8_t);
MSGT_CREATE_SIMPLE_TRAITS(uint16_t);
MSGT_CREATE_SIMPLE_TRAITS(int16_t);
MSGT_CREATE_SIMPLE_TRAITS(uint32_t);
MSGT_CREATE_SIMPLE_TRAITS(int32_t);
MSGT_CREATE_SIMPLE_TRAITS(uint64_t);
MSGT_CREATE_SIMPLE_TRAITS(int64_t);
MSGT_CREATE_SIMPLE_TRAITS(float);
MSGT_CREATE_SIMPLE_TRAITS(double);

// because std::vector<bool> is not a true vector, bool is not a simple type
template<> struct IsFixedSize<bool> : public TrueType {};


}
}

#endif //MESSAGE_TRANSPORT_MESSAGE_TRAITS_H

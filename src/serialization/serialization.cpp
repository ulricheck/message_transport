//
// Created by Ulrich Eck on 07/05/16.
//

#include "message_transport/serialization/serialization.h"

namespace message_transport {

	void throwStreamOverrun() {
		throw StreamOverrunException("Buffer Overrun");
	}

}
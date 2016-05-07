#include <iostream>
#include <boost/thread.hpp>
#include "message_transport/sharedmem/SharedMemoryBlock.h"

using namespace boost::interprocess;
using namespace sharedmem_transport;

boost::mutex main_mutex;
managed_shared_memory* segment = NULL;
SharedMemoryBlock* blockmgr = NULL;

// these methods should be exposed as part of the library
bool release_memory(const char* name)
{
    boost::lock_guard<boost::mutex> guard(main_mutex);//auto-lock unlock, even on exception
    shm_handle shm = blockmgr->findHandle(*segment, name);
    if (!shm.is_valid()) {
        //LOG_ERROR("Trying to release block %s not managed by sharedmem_manager", name);
        return false;
    }
    else {
        blockmgr->resetBlock(*segment, shm);
        //LOG_INFO("Released block %s", name);
    }
    return true;
}

bool clear_memory()
{
    boost::lock_guard<boost::mutex> guard(main_mutex);//auto-lock unlock, even on exception
    blockmgr->resetAllBlocks(*segment);
    //LOG_INFO("Deleted all handles");
    return true;
}
//
//bool get_blocks(std::vector<SharedMemBlock>& result)
//{
//    boost::lock_guard<boost::mutex> guard(main_mutex);//auto-lock unlock, even on exception
//    result = blockmgr->getBlockList();
//    return true;
//}

int main(int argc, char* argv[])
{
    std::string segment_name = MSGTSharedMemoryDefaultBlock;
    int segment_size = 1000000;

    // @todo handle commandline arguments for sharedmem_manager
    // segment_name
    // segment_size

    //Remove shared memory on construction and destruction
    struct shm_remove {
      std::string name_;
      shm_remove(const std::string& name)
              :name_(name)
      {
          shared_memory_object::remove(name_.c_str());
      }
      ~shm_remove()
      {
          //LOG_INFO("Destroying shared memory object");
          shared_memory_object::remove(name_.c_str());
      }
    } remover(segment_name);

    //Managed memory segment that allocates portions of a shared memory
    //segment with the default management algorithm
    managed_shared_memory managed_shm(create_only, segment_name.c_str(), segment_size);
    segment = &managed_shm;
    blockmgr = segment->find_or_construct<SharedMemoryBlock>("Manager")();
    //LOG_INFO("Created segment %p, and constructed block %p", segment, blockmgr);

    // XXX Advertise service availability here ..
    //LOG_INFO("Created shared memory with %d bytes", segment_size);

    // XXX NEED SOME WAY TO KEEP THE PROCESS RUNNING UNTIL ALL IS DONE.
    std::cin.ignore();


    //LOG_INFO("Exiting sharedmem_manager, block %p",blockmgr);
    if (blockmgr) {
        //LOG_INFO("Clearing all shared memory blocks");
        blockmgr->resetAllBlocks(*segment);
        blockmgr = NULL;
    }
}




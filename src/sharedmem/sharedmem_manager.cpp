#include <iostream>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include "message_transport/sharedmem/SharedMemoryBlock.h"
#include "message_transport/logging.h"

using namespace boost::interprocess;
using namespace sharedmem_transport;

boost::mutex main_mutex;
managed_shared_memory* segment = NULL;
SharedMemoryBlock* blockmgr = NULL;

// these methods should be exposed as part of the library
//bool release_memory(const char* name)
//{
//    boost::lock_guard<boost::mutex> guard(main_mutex);//auto-lock unlock, even on exception
//    shm_handle shm = blockmgr->findHandle(*segment, name);
//    if (!shm.is_valid()) {
//        LOG_ERROR("Trying to release block %s not managed by sharedmem_manager", name);
//        return false;
//    }
//    else {
//        blockmgr->resetBlock(*segment, shm);
//        LOG_INFO("Released block %s", name);
//    }
//    return true;
//}
//
//bool clear_memory()
//{
//    boost::lock_guard<boost::mutex> guard(main_mutex);//auto-lock unlock, even on exception
//    blockmgr->resetAllBlocks(*segment);
//    LOG_INFO("Deleted all handles");
//    return true;
//}
//
//bool get_blocks(std::vector<SharedMemBlock>& result)
//{
//    boost::lock_guard<boost::mutex> guard(main_mutex);//auto-lock unlock, even on exception
//    result = blockmgr->getBlockList();
//    return true;
//}

int main(int argc, char **argv)
{
    std::string segment_name = MSGTSharedMemoryDefaultBlock;
    int segment_size = 1000000;

    try {

        // describe program options
        namespace po = boost::program_options;
        po::options_description poDesc("Allowed options", 80);
        poDesc.add_options()
                ("help", "print this help message")
                ("size", po::value<int>(&segment_size), "Segment Size")
                ("name", po::value<std::string>(&segment_name), "Segment Name");

        // specify default options
        po::positional_options_description inputOptions;
        //inputOptions.add( "utql", 1 );

        // parse options from command line and environment
        po::variables_map poOptions;
        po::store(po::command_line_parser(argc, argv).options(poDesc).positional(inputOptions).run(), poOptions);
        po::store(po::parse_environment(poDesc, "MSGT_DEFAULTS_"), poOptions);
        po::notify(poOptions);

        // print help message if nothing specified
        if (poOptions.count("help")) {
            std::cout << "Syntax: msgt_manager [options] " << std::endl << std::endl;
            std::cout << poDesc << std::endl;
            return 0;
        }
    }
    catch (std::exception &e) {
        std::cerr << "Error parsing command line parameters : " << e.what() << std::endl;
        std::cerr << "Try msgt_manager --help for help" << std::endl;
        return 1;
    }

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
          LOG_INFO("Destroying shared memory object");
          shared_memory_object::remove(name_.c_str());
      }
    } remover(segment_name);

    //Managed memory segment that allocates portions of a shared memory
    //segment with the default management algorithm
    managed_shared_memory managed_shm(create_only, segment_name.c_str(), segment_size);
    segment = &managed_shm;
    blockmgr = segment->find_or_construct<SharedMemoryBlock>("Manager")();
    LOG_INFO("Created segment " << segment <<", and constructed block " << blockmgr);

    // XXX Advertise service availability here ..
    LOG_INFO("Created shared memory with " << segment_size << " bytes" );

    // XXX NEED SOME WAY TO KEEP THE PROCESS RUNNING UNTIL ALL IS DONE.
    std::cout << "Press enter to exit." << std::endl;
    std::cin.ignore();


    LOG_INFO("Exiting sharedmem_manager, block " << blockmgr);
    if (blockmgr) {
        LOG_INFO("Clearing all shared memory blocks");
        blockmgr->resetAllBlocks(*segment);
        blockmgr = NULL;
    }
}




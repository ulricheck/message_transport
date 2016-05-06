#ifndef SHARED_MEMORY_BLOCK_H
#define SHARED_MEMORY_BLOCK_H

#include <cassert>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "message_transport/sharedmem/SharedMemoryBlockDescriptor.h"
#include "message_transport/sharedmem/SharedMemoryBlock.h"

namespace sharedmem_transport {

#define MSGTSharedMemoryDefaultBlock "MessageTransport::SharedMemoryBlock"
#define MSGTSharedMemoryNumBlock 100



    struct shm_handle {
        uint32_t handle;
        uint8_t *ptr;
        uint32_t resize_count;
        shm_handle() : handle(-1), ptr(NULL), resize_count(0) {}
        shm_handle(uint32_t h, uint32_t rcount, uint8_t *p) : handle(h), ptr(p), resize_count(rcount) {}
        bool is_valid() const {return ptr != NULL;}
    };

    class SharedMemoryBlock {
        protected:
            //Mutex to protect access to the queue
            boost::interprocess::interprocess_mutex      mutex;
            boost::interprocess::interprocess_condition  cond;
            int32_t num_clients;
            SharedMemoryBlockDescriptor descriptors[MSGTSharedMemoryNumBlock];


        public:

            SharedMemoryBlock() : num_clients(0) {}

            shm_handle findHandle(boost::interprocess::managed_shared_memory & segment, const char * name) ;


            shm_handle allocateBlock(boost::interprocess::managed_shared_memory & segment, 
                    const char * name, uint32_t size) ;

            void resetBlock(boost::interprocess::managed_shared_memory & segment, shm_handle & shm) ;
            void resetAllBlocks(boost::interprocess::managed_shared_memory & segment) ;

            void reallocateBlock(boost::interprocess::managed_shared_memory & segment, 
                    shm_handle & shm, uint32_t size) ;

            template <class Base>
            bool wait_data(boost::interprocess::managed_shared_memory & segment,
                    shm_handle & src, Base & msg) {
                {
                    //LOG_DEBUG("Locking %d",src.handle);
                    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
                        lock(descriptors[src.handle].mutex);
                    if (!descriptors[src.handle].wait_data_and_register_client(lock))  {
                        return false;
                    }
                    /*
                    if (!ros::ok()) {
                        lock.unlock();
                        descriptors[src.handle].unregister_client();
                        return false;
                    }
                    */
                    register_global_client();
                    //LOG_DEBUG("Unlocking %d",src.handle);
                }
                // @todo Implement serialization for Message Types
//                deserialize<Base>(segment,src,msg);

                unregister_global_client();
                //LOG_DEBUG("Unregistering %d",src.handle);
                descriptors[src.handle].unregister_client();
                //if (!ros::ok()) return false;
                return true;
            }

            template <class M>
            void serialize(boost::interprocess::managed_shared_memory & segment,
                    shm_handle & dest, const M & msg) {
                boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(descriptors[dest.handle].mutex);
                //LOG_DEBUG("serialize: locked %d, checking clients",dest.handle);
                descriptors[dest.handle].check_clients(lock);
                //LOG_DEBUG("serialize: locked %d, clients checked",dest.handle);
                register_global_client();
                //LOG_DEBUG("serialize: global clients checked");

                assert(dest.handle < MSGTSharedMemoryNumBlock);
                if (dest.resize_count != descriptors[dest.handle].resize_count_) {
                    std::pair<uint8_t *, std::size_t> ret = segment.find<uint8_t>(descriptors[dest.handle].name_);
                    dest.resize_count = descriptors[dest.handle].resize_count_;
                    dest.ptr = ret.first;
                }
                //LOG_DEBUG("Serialising to %p, %d bytes",dest.ptr,descriptors[dest.handle].size_);

                // @todo Imlement deserialization for Message Types
//                ros::serialization::OStream out(dest.ptr,descriptors[dest.handle].size_);
//                ros::serialization::serialize(out, msg);

                unregister_global_client();
                //LOG_DEBUG("serialize: global clients released");
                descriptors[dest.handle].signal_data();
                //LOG_DEBUG("serialize: unlocking %d",dest.handle);
            }

            //std::vector<SharedMemBlock> getBlockList() const ;
        protected:

            template <class Base>
            void deserialize(boost::interprocess::managed_shared_memory & segment,
                    shm_handle & src, Base & msg) {
                assert(src.handle < MSGTSharedMemoryNumBlock);
                if (src.resize_count != descriptors[src.handle].resize_count_) {
                    std::pair<uint8_t *, std::size_t> ret = segment.find<uint8_t>(descriptors[src.handle].name_);
                    src.resize_count = descriptors[src.handle].resize_count_;
                    src.ptr = ret.first;
                }
                //LOG_DEBUG("Deserialising from %p, %d bytes",src.ptr,descriptors[src.handle].size_);

                // @todo Implement serialization for Message Types
//                ros::serialization::IStream in(src.ptr,descriptors[src.handle].size_);
//                ros::serialization::deserialize(in, msg);
            }

            
            shm_handle connectBlock(boost::interprocess::managed_shared_memory & segment, uint32_t handle) ;

            void check_global_clients(boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> & lock) {
                if (num_clients) {
                    //LOG_DEBUG("Lock_global wait");
                    cond.wait(lock);
                }
                //LOG_DEBUG("Lock_global done");
            }


            void register_global_client() {
                //LOG_DEBUG("register_global_client:: Locking global");
                boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex); 
                num_clients ++;
                //LOG_DEBUG("Registered global client");
            }

            void unregister_global_client() {
                //LOG_DEBUG("unregister_global_client:: Locking global");
                boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex); 
                num_clients --;
                assert(num_clients >= 0);
                if (num_clients == 0) {
                    //LOG_DEBUG("Global lock is free");
                    cond.notify_all();
                }
                //LOG_DEBUG("Unregistered global client");
            }


    };
} //namespace image_transport

#endif // SHARED_MEMORY_BLOCK_H

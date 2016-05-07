#include "message_transport/sharedmem/SharedMemoryBlock.h"

using namespace boost::interprocess;

namespace sharedmem_transport {

    shm_handle SharedMemoryBlock::connectBlock(boost::interprocess::managed_shared_memory & segment, uint32_t handle) {
        assert(handle < MSGTSharedMemoryNumBlock);
        std::pair<uint8_t *, std::size_t> ret = segment.find<uint8_t>(descriptors[handle].name_);
        LOG_DEBUG("Connect block " << handle << ": handle " << ret.first << " size " << ret.second);
        assert(ret.second >= descriptors[handle].size_);
        return shm_handle(handle,descriptors[handle].resize_count_,ret.first);
    }

    shm_handle SharedMemoryBlock::findHandle(boost::interprocess::managed_shared_memory & segment, const char * name) {
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
        check_global_clients(lock);
        LOG_DEBUG("Find handle: searching " << name);
        for (int32_t i=0;i<MSGTSharedMemoryNumBlock;i++) {
            if (!descriptors[i].active_) {
                LOG_DEBUG("Find handle " << i << " inactive");
                continue;
            }
            LOG_DEBUG("Find handle " << i << ": " << descriptors[i].name_);
            if (descriptors[i].matchName(name)) {
                return connectBlock(segment,i);
            }
        }
        LOG_DEBUG("Find handle: not found");
        return shm_handle();
    }

    shm_handle SharedMemoryBlock::allocateBlock(boost::interprocess::managed_shared_memory & segment, 
            const char * name, uint32_t size) {
        uint32_t i;
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
        check_global_clients(lock);
        for (i=0;i<MSGTSharedMemoryNumBlock;i++) {
            if (!descriptors[i].active_) continue;
            if (descriptors[i].matchName(name)) {
                std::pair<uint8_t *, std::size_t> ret = segment.find<uint8_t>(descriptors[i].name_);
                uint8_t *ptr = ret.first;
                if (size > ret.second) {
                    segment.destroy<uint8_t>(descriptors[i].name_);
                    ptr = segment.find_or_construct<uint8_t>(descriptors[i].name_)[size](0);
                    descriptors[i].recordSize(size,size);
                } else {
                    descriptors[i].recordSize(size,ret.second);
                }
                LOG_INFO("allocateBlock: reconnected block " << name << " to handle " << i << ", ptr " << ptr);
                return shm_handle(i,descriptors[i].resize_count_,ptr);
            }
        }
        // new object
        for (i=0;i<MSGTSharedMemoryNumBlock;i++) {
            if (descriptors[i].active_) continue;
            descriptors[i].allocate(name);
            descriptors[i].recordSize(size,size);
            uint8_t * ptr = segment.find_or_construct<uint8_t>(descriptors[i].name_)[size](0);
            LOG_INFO("allocateBlock: connected block " << name << " to handle " << i << ", ptr " << ptr);
            return shm_handle(i,descriptors[i].resize_count_,ptr);
        }

        LOG_INFO("allocateBlock: No free block for " << name);
        return shm_handle();
    }

    void SharedMemoryBlock::resetBlock(boost::interprocess::managed_shared_memory & segment, shm_handle & shm) {
        scoped_lock<interprocess_mutex> lock(descriptors[shm.handle].mutex);
        descriptors[shm.handle].check_clients(lock);
        scoped_lock<interprocess_mutex> lockg(mutex);
        check_global_clients(lockg);
        if (shm.is_valid()) {
            assert(shm.handle < MSGTSharedMemoryNumBlock);
            segment.destroy<uint8_t>(descriptors[shm.handle].name_);
            descriptors[shm.handle].reset();
            shm = shm_handle();
        }
    }

    void SharedMemoryBlock::resetAllBlocks(boost::interprocess::managed_shared_memory & segment) {
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
        check_global_clients(lock);
        for (uint32_t i=0;i<MSGTSharedMemoryNumBlock;i++) {
            if (!descriptors[i].active_) continue;
            segment.destroy<uint8_t>(descriptors[i].name_);
            descriptors[i].reset();
        }
    }

    void SharedMemoryBlock::reallocateBlock(boost::interprocess::managed_shared_memory & segment, 
            shm_handle & shm, uint32_t size) {
        if (!shm.is_valid()) {
            LOG_ERROR("Can't reallocateBlock associated to invalid handle");
            return;
        }
        scoped_lock<interprocess_mutex> lock(descriptors[shm.handle].mutex);
        LOG_DEBUG("reallocateBlock: locked " << shm.handle << ", checking clients");
        descriptors[shm.handle].check_clients(lock);
        LOG_DEBUG("reallocateBlock: locked " << shm.handle << ", clients checked");
        // First check if the block has been changed since last access, and
        // update the pointer if necessary
        if (shm.resize_count != descriptors[shm.handle].resize_count_) {
            scoped_lock<interprocess_mutex> lockg(mutex);
            LOG_DEBUG("reallocateBlock: locked global, checking clients");
            check_global_clients(lockg);
            LOG_DEBUG("reallocateBlock: locked global, client checked");
            std::pair<uint8_t *, std::size_t> ret = segment.find<uint8_t>(descriptors[shm.handle].name_);
            shm.resize_count = descriptors[shm.handle].resize_count_;
            shm.ptr = ret.first;
            LOG_DEBUG("reallocateBlock: unlocking global");
        }
        // Then check if the size need to be expanded
        if (size > descriptors[shm.handle].allocated_) {
            scoped_lock<interprocess_mutex> lockg(mutex);
            LOG_DEBUG("reallocateBlock2: locked global, checking clients");
            check_global_clients(lockg);
            LOG_INFO("Reallocating block " << descriptors[shm.handle].name_ << ":" << shm.ptr << " from " <<
                    descriptors[shm.handle].allocated_ << " to " << size << " bytes");
            segment.destroy<uint8_t>(descriptors[shm.handle].name_);
            try {
                uint8_t * ptr = segment.find_or_construct<uint8_t>(descriptors[shm.handle].name_)[size](0);
                shm.ptr = ptr;
                descriptors[shm.handle].recordSize(size,size);
                shm.resize_count = descriptors[shm.handle].resize_count_;
            } catch (boost::interprocess::bad_alloc e) {
                LOG_ERROR("Failed to reallocate memory block, resetting block");
                descriptors[shm.handle].reset();
                shm = shm_handle();
            }
            LOG_DEBUG("reallocateBlock2: unlocking global");
        } else {
            descriptors[shm.handle].recordSize(size);
        }
        LOG_DEBUG("reallocateBlock: unlocking " << shm.handle);
    }

//    std::vector<SharedMemBlock> SharedMemoryBlock::getBlockList() const {
//        unsigned int i;
//        std::vector<SharedMemBlock> out;
//        for (i=0;i<MSGTSharedMemoryNumBlock;i++) {
//            if (!descriptors[i].active_) continue;
//            SharedMemBlock block;
//            block.handle = i;
//            block.size = descriptors[i].size_;
//            block.allocated = descriptors[i].allocated_;
//            block.name = descriptors[i].name_;
//            out.push_back(block);
//        }
//        return out;
//    }

} //namespace image_transport


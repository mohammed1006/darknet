#pragma once

#include <stdlib.h>

#include "types/enum.hpp"
#include "utils/memory.hpp"


namespace darknet
{
namespace tensor
{
    template<DeviceType device>
    class Allocator
    {
    private:
        /* data */
    public:
        void* allocate(size_t num_bytes);
        void* allocate(cuda::device_t* device, size_t num_bytes);
        template<DataType T>
        void* allocate(size_t num_elem)
        {
            return allocate(utils::num_bytes(T) * num_elem);
        }

        void free(void* ptr);
        template<typename T>
        void free(T* ptr)
        {
            free(std::static_pointer_cast<void*>(ptr));
        }
    };
} // namespace tensor
} // namespace darknet

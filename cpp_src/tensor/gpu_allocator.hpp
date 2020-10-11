#pragma once

#include "tensor/allocator.hpp"
#include "cuda/api_wrappers.hpp"


namespace darknet
{
namespace tensor
{


    class GpuAllocator : Allocator<DeviceType::GPUDEVICE>
    {
    private:
        cuda::device_t device;
    public:
        GpuAllocator::GpuAllocator(cuda::device_t device) : device(device)
        {

        }

        GpuAllocator::~GpuAllocator()
        {

        }

        void* GpuAllocator::allocate(size_t num_bytes)
        {
            return device.memory().allocate(num_bytes);
        }

        void GpuAllocator::free(void* ptr)
        {
            cuda::memory::device::free(ptr);
        }
    };

} // namespace tensor
} // namespace darknet

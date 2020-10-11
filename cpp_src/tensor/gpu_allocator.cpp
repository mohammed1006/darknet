#include "tensor/allocator.hpp"

#define GpuAllocator Allocator<DeviceType::GPUDEVICE>

namespace darknet
{
namespace tensor
{

    // void* GpuAllocator::allocate(size_t num_bytes)
    // {
    //     return device.memory().allocate(num_bytes);
    // }

    // template<>
    // void GpuAllocator::free(void* ptr)
    // {
    //       cuda::memory::device::free(ptr);
    // }

} // namespace tensor
} // namespace darknet

#undef GpuAllocator
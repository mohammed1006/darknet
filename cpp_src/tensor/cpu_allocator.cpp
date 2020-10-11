
#include "tensor/allocator.hpp"

#define CpuAllocator Allocator<DeviceType::CPUDEVICE>

namespace darknet
{
namespace tensor
{

    void* CpuAllocator::allocate(size_t num_bytes)
    {
        return std::malloc(num_bytes);
    }

    template<>
    void CpuAllocator::free(void* ptr)
    {
        std::free(ptr);
    }

} // namespace tensor
} // namespace darknet

#undef CpuAllocator
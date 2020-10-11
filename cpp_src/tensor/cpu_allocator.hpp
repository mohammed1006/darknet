#pragma once

#include "tensor/allocator.hpp"


namespace darknet
{
namespace tensor
{

    // class CpuAllocator : Allocator<DeviceType::CPUDEVICE>
    // {
    // private:
    // public:
    //     CpuAllocator::CpuAllocator()
    //     {

    //     }

    //     CpuAllocator::~CpuAllocator()
    //     {

    //     }

    //     void* CpuAllocator::allocate(size_t num_bytes)
    //     {
    //         return std::malloc(num_bytes);
    //     }

    //     void CpuAllocator::free(void* ptr)
    //     {
    //         std::free(ptr);
    //     }

    //     // CpuAllocator();
    //     // ~CpuAllocator();
    //     // void* allocate(size_t num_bytes) override;
    //     // void free(void* ptr) override;
    // };

} // namespace tensor
} // namespace darknet

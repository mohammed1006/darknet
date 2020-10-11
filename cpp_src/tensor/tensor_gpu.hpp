#pragma once

#include <iostream>

#include "tensor/tensor.hpp"
#include "cuda/api_wrappers.hpp"

namespace darknet
{
namespace tensor
{

    template<typename T>
    class Tensor<T, DeviceType::GPUDEVICE> : public TensorBase<T, DeviceType::CPUDEVICE>
    {
    private:
        cuda::device_t _device;
    public:

        Tensor() : _device(cuda::device::current::get())
        {
            this->data = nullptr;
        }

        Tensor(TensorShape& shape) : TensorBase<T, DeviceType::CPUDEVICE>(shape), _device(cuda::device::current::get())
        {
            this->data = static_cast<T*>(_device.memory().allocate(shape.numElem() * sizeof(T)));
        }

        ~Tensor()
        {
            if(this->data)
            {
                cuda::memory::device::free(this->data);
            }
        }
    };
    
} // namespace tensor
} // namespace darknet

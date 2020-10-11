#pragma once

#include <algorithm>

#include "tensor/tensor.hpp"

namespace darknet
{
namespace tensor
{

    /**
     * @brief CPU tensor
     * 
     * @tparam T 
     */
    template<typename T>
    class Tensor<T, DeviceType::CPUDEVICE> : public TensorBase<T, DeviceType::CPUDEVICE>
    {
    private:
        
    public:
        Tensor() : TensorBase<T, DeviceType::CPUDEVICE>(TensorShape({}))
        {

        }

        Tensor(TensorShape& shape) : TensorBase<T, DeviceType::CPUDEVICE>(shape)
        {
            this->data = static_cast<T*>(std::malloc(shape.numElem() * sizeof(T)));
        }

        ~Tensor()
        {
            if(this->data)
            {
                std::free(this->data);
            }
        }

        Tensor operator+(Tensor& other)
        {
            
        }
        Tensor operator*(Tensor& other)
        {

        }

    };
    
} // namespace tensor
} // namespace darknet

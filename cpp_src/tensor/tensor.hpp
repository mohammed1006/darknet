#pragma once

#include <memory>
#include <ostream>

#include "types/enum.hpp"
#include "tensor/tensor_shape.hpp"
#include "tensor/tensor_base.hpp"


namespace darknet
{
namespace tensor
{

    template<typename T, DeviceType device>
    class Tensor : TensorBase<T, device>
    {
    protected:

        
    public:

        Tensor operator+(Tensor& other);
        Tensor operator*(Tensor& other);

        // template<typename T1, DeviceType device1>
        // friend std::ostream& operator<< (std::ostream& out, const Tensor<T1, device1>& obj);
    };
    
    // template<typename T, DeviceType device>
    // std::ostream& operator<< (std::ostream& out, const Tensor<T, device>& obj)
    // {
    //     out << "<Tensor " << obj.dtype << " " << obj.shape << ">";
    //     return out;
    // }
} // namespace tensor
} // namespace darknet

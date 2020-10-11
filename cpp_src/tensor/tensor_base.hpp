#pragma once

#include <memory>
#include <ostream>

#include "types/enum.hpp"
#include "tensor/tensor_shape.hpp"

namespace darknet
{
namespace tensor
{

    /**
     * @brief Base class for multi dimensional arrays. This has data and dimension info.
     * 
     * @tparam T Type of data
     * @tparam device device to store data and execute operations on
     */
    template<typename T, DeviceType device>
    class TensorBase
    {
    protected:
        DataType dtype;
        T* data = nullptr;
        TensorShape shape;

        /**
         * @brief Hide the constructor so you can't create just the base class. Use Tensor instead
         * 
         * @param shape shape to create
         */
        TensorBase(TensorShape& shape) : shape(shape)
        {

        }

    public:

        /**
         * @brief Get the Device of this tensor
         * 
         * @return DeviceType the device
         */
        DeviceType getDevice() {return device;}

        /**
         * @brief Get the pointer to the data. Caution: this can be pointing to cpu or gpu memory.
         * 
         * @return T* 
         */
        T* ptr() {return data;}

        // template<typename T1, DeviceType device1>
        // friend std::ostream& operator<< (std::ostream& out, const TensorBase<T1, device1>& obj);
    };
    
    // template<typename T, DeviceType device>
    // std::ostream& operator<< (std::ostream& out, const TensorBase<T, device>& obj)
    // {
    //     out << "<TensorBase " << obj.dtype << " " << obj.shape << ">";
    //     return out;
    // }
} // namespace tensor
} // namespace darknet

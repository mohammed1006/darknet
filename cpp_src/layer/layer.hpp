#pragma once

#include <memory>
#include <vector>

#include "types/enum.hpp"
#include "network/network_state.hpp"
#include "tensor/tensor.hpp"

namespace darknet
{
namespace layer
{
    /**
     * @brief Basic layer class to be inherited.
     * TODO: need logic for shapes and input size verification.
     */
    class Layer
    {
    protected:
        std::shared_ptr<Layer> inputLayer;
        /**
         * @brief Function for checking that the input shape is compatable
         * Analogous to parts of make_*_layer
         */
        virtual void verifyShape() = 0;
        /**
         * @brief Initialize any memory (GPU or CPU) that is required.
         * Analogous to parts of make_*_layer
         */
        virtual void init() = 0;


    public:
        Layer(std::shared_ptr<Layer> inputLayer, LayerType type);

        /**
         * @brief Compute the forward pass of this layer
         * 
         * Analogous to layer.froward[_gpu] function pointer
         */
        virtual void forward(std::shared_ptr<network::NetworkState>& netState) = 0;

        /**
         * @brief Compute the backwards pass (gradients) of this layer
         * Analogous to layer.backward[_gpu] function pointer
         */
        virtual void backward(std::shared_ptr<network::NetworkState>& netState) = 0;
        /**
         * @brief 
         * Analogous to layer.update[_gpu] function pointer
         */
        virtual void update(int, float, float, float) = 0;
        /**
         * @brief Resize this layer if possible.
         * Analogous to resize_*_layer
         */
        virtual void resize() = 0;

        // The type of this layer. i.e. conv or lstm
        const LayerType type;
        
        // The output tensor for this layer.
        std::shared_ptr<tensor::Tensor<float>> output;
    };

    
} // namespace layer
} // namespace darknet
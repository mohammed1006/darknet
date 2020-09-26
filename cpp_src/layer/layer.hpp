#pragma once

#include <memory>
#include <vector>

#include "types/enum.hpp"
#include "network/network_state.hpp"

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

    public:
        Layer(std::shared_ptr<Layer> inputLayer, LayerType type);

        /**
         * @brief Compute the forward pass of this layer
         * 
         */
        virtual void forward(std::shared_ptr<network::NetworkState>& netState) = 0;

        /**
         * @brief Compute the backwards pass (gradients) of this layer
         * 
         */
        virtual void backward(std::shared_ptr<network::NetworkState>& netState) = 0;
        virtual void update(int, float, float, float) = 0;
        const LayerType type;
        std::vector<float> output;
    };

    
} // namespace layer
} // namespace darknet
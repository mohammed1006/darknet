#include "layer/convolutional.hpp"

namespace darknet
{
namespace layer
{
    ConvolutionalLayer::ConvolutionalLayer(std::shared_ptr<Layer> inputLayer)
        : Layer(inputLayer, LayerType::CONVOLUTIONAL)
    {

    }
} // namespace layer
} // namespace darknet

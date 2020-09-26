#pragma once

#include <layer/layer.hpp>

namespace darknet
{
namespace layer
{
class ConvolutionalLayer : Layer
{
private:

public:
    ConvolutionalLayer(std::shared_ptr<Layer> inputLayer);

};
    
} // namespace layer
} // namespace darknet
#include <layer/layer.hpp>

namespace darknet
{
namespace layer
{

    Layer::Layer(std::shared_ptr<Layer> inputLayer, LayerType type)
        : inputLayer(inputLayer), type(type)
    {
        verifyShape();
        init();
    }
    
} // namespace layer
} // namespace darknet
#include <layer/activation.hpp>

namespace darknet
{
namespace layer
{
    
    Activation::Activation(std::shared_ptr<Layer> inputLayer, ActivationType actType)
        : actType(actType), Layer(inputLayer, LayerType::ACTIVE)
    {
        
    }
    
    Activation::Activation(std::shared_ptr<Layer> input, std::string& actType)
        : Activation(input, fromString(actType))
    {

    }


    void Activation::forward(std::shared_ptr<network::NetworkState>& netState)
    {
        output.resize(inputLayer->output.size());
        std::copy(inputLayer->output.begin(), inputLayer->output.begin(), output.begin());

    }

    void Activation::backward(std::shared_ptr<network::NetworkState>& netState)
    {

    }
    void Activation::update(int, float, float, float)
    {

    }

    void Activation::activateOnOutput()
    {
        int i;
        if (actType == LINEAR) {}
        else if (actType == LEAKY) {
            #pragma omp parallel for
            for (i = 0; i < output.size(); ++i) {
                output[i] = leaky(output[i]);
            }
        }
        else if (actType == LOGISTIC) {
            #pragma omp parallel for
            for (i = 0; i < output.size(); ++i) {
                output[i] = logistic(output[i]);
            }
        }
        else {
            for (i = 0; i < output.size(); ++i) {
                output[i] = activate(output[i]);
            }
        }
    }

    float Activation::activate(float x)
    {
        switch(actType){
            case LINEAR:
                return linear(x);
            case LOGISTIC:
                return logistic(x);
            case LOGGY:
                return loggy(x);
            case RELU:
                return relu(x);
            case ELU:
                return elu(x);
            case SELU:
                return selu(x);
            case GELU:
                return gelu(x);
            case RELIE:
                return relie(x);
            case RAMP:
                return ramp(x);
            case REVLEAKY:
            case LEAKY:
                return leaky(x);
            case TANH:
                return tanh(x);
            case PLSE:
                return plse(x);
            case STAIR:
                return stair(x);
            case HARDTAN:
                return hardtan(x);
            case LHTAN:
                return lhtan(x);
        }
        return 0;
    }

} // namespace layer
} // namespace darknet

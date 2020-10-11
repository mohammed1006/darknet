#pragma once

#include <cmath>
#include <string>
#include <iostream>

#include "layer/layer.hpp"
#include "types/enum.hpp"
#include "ops/activation.hpp"

namespace darknet
{
namespace layer
{
    class Activation : Layer
    {
    protected:
        /******************************************************
         * Activation functions                               *
         ******************************************************/
        static ActivationType fromString(std::string& s)
        {
            if (s == "logistic") return ActivationType::LOGISTIC;
            if (s == "swish") return ActivationType::SWISH;
            if (s == "mish") return ActivationType::MISH;
            if (s == "hard_mish") return ActivationType::HARD_MISH;
            if (s == "normalize_channels") return ActivationType::NORM_CHAN;
            if (s == "normalize_channels_softmax") return ActivationType::NORM_CHAN_SOFTMAX;
            if (s == "normalize_channels_softmax_maxval") return ActivationType::NORM_CHAN_SOFTMAX_MAXVAL;
            if (s == "loggy") return ActivationType::LOGGY;
            if (s == "relu") return ActivationType::RELU;
            if (s == "relu6") return ActivationType::RELU6;
            if (s == "elu") return ActivationType::ELU;
            if (s == "selu") return ActivationType::SELU;
            if (s == "gelu") return ActivationType::GELU;
            if (s == "relie") return ActivationType::RELIE;
            if (s == "plse") return ActivationType::PLSE;
            if (s == "hardtan") return ActivationType::HARDTAN;
            if (s == "lhtan") return ActivationType::LHTAN;
            if (s == "linear") return ActivationType::LINEAR;
            if (s == "ramp") return ActivationType::RAMP;
            if (s == "revleaky") return ActivationType::REVLEAKY;
            if (s == "leaky") return ActivationType::LEAKY;
            if (s == "tanh") return ActivationType::TANH;
            if (s == "stair") return ActivationType::STAIR;
            std::cerr << "Couldn't find activation function " << s << ", going with ReLU" << std::endl;
            return ActivationType::RELU;
        }
        static inline float stair(float x)
        {
            int n = std::floor(x);
            if (n%2 == 0) return std::floor(x/2.f);
            else return (x - n) + std::floor(x/2.f);
        }
        static inline float hardtan(float x)
        {
            if (x < -1) return -1;
            if (x > 1) return 1;
            return x;
        }
        static inline float linear(float x){return x;}
        static inline float logistic(float x){return 1.f/(1.f + std::exp(-x));}
        static inline float loggy(float x){return 2.f/(1.f + std::exp(-x)) - 1;}
        static inline float relu(float x){return x*(x>0);}
        static inline float relu6(float x) { return std::min(std::max(x, 0.0f), 6.0f); }
        static inline float elu(float x){return (x >= 0)*x + (x < 0)*(std::exp(x)-1);}
        static inline float selu(float x) { return (x >= 0)*1.0507f*x + (x < 0)*1.0507f*1.6732f*(std::exp(x) - 1); }
        static inline float relie(float x){return (x>0) ? x : .01f*x;}
        static inline float ramp(float x){return x*(x>0)+.1f*x;}
        static inline float leaky(float x){return (x>0) ? x : .1f*x;}
        static inline float tanh(float x) { return (2 / (1 + std::exp(-2 * x)) - 1); }
        static inline float gelu(float x) { return (0.5*x*(1 + std::tanh(0.797885*x + 0.035677*std::pow(x, 3)))); }
        static inline float softplus(float x, float threshold) {
            if (x > threshold) return x;                // too large
            else if (x < -threshold) return std::exp(x);    // too small
            return std::log(std::exp(x) + 1);
        }
        static inline float plse(float x)
        {
            if(x < -4) return .01f * (x + 4);
            if(x > 4)  return .01f * (x - 4) + 1;
            return .125f*x + .5f;
        }

        static inline float lhtan(float x)
        {
            if(x < 0) return .001f*x;
            if(x > 1) return .001f*(x-1) + 1;
            return x;
        }
        static inline float lhtan_gradient(float x)
        {
            if(x > 0 && x < 1) return 1;
            return .001f;
        }

        static inline float hardtan_gradient(float x)
        {
            if (x > -1 && x < 1) return 1;
            return 0;
        }
        static inline float linear_gradient(float x){return 1;}
        static inline float logistic_gradient(float x){return (1-x)*x;}
        static inline float loggy_gradient(float x)
        {
            float y = (x+1.f)/2.f;
            return 2*(1-y)*y;
        }
        static inline float stair_gradient(float x)
        {
            if (floor(x) == x) return 0;
            return 1.0f;
        }
        static inline float relu_gradient(float x){return (x>0);}
        static inline float relu6_gradient(float x) { return (x > 0 && x < 6); }
        static inline float elu_gradient(float x){return (x >= 0) + (x < 0)*(x + 1);}
        static inline float selu_gradient(float x) { return (x >= 0)*1.0507f + (x < 0)*(x + 1.0507f*1.6732f); }
        static inline float relie_gradient(float x){return (x>0) ? 1 : .01f;}
        static inline float ramp_gradient(float x){return (x>0)+.1f;}
        static inline float leaky_gradient(float x){return (x>0) ? 1 : .1f;}
        static inline float tanh_gradient(float x){return 1-x*x;}

        static inline float sech(float x) { return 2 / (std::exp(x) + std::exp(-x)); }
        static inline float gelu_gradient(float x) {
            const float x3 = std::pow(x, 3);
            return 0.5*std::tanh(0.0356774*x3 + 0.797885*x) + (0.0535161*x3 + 0.398942*x) * std::pow(sech(0.0356774*x3 + 0.797885*x), 2) + 0.5;
        }
        static inline float plse_gradient(float x){return (x < 0 || x > 1) ? .01f : .125f;}


        /******************************************************
         * Array operations                                   *
         ******************************************************/
        void activateOnOutput();
        float activate(float x);

        ActivationType actType;
    public:
        /**
         * @brief Construct a new Activation layer
         * 
         * @param activationType Type of Activation
         */
        Activation(std::shared_ptr<Layer> input, ActivationType actType);
        Activation(std::shared_ptr<Layer> input, std::string& actType);
        void forward(std::shared_ptr<network::NetworkState>& netState) override;
        void backward(std::shared_ptr<network::NetworkState>& netState) override;
        void update(int, float, float, float) override;
    };

} // namespace layer
} // namespace darknet
#pragma once

#include <ostream>

namespace darknet
{
    enum DeviceType {
        CPUDEVICE,
        GPUDEVICE
    };

    enum DataType {
        DT_FLOAT16,
        DT_FLOAT32,
        DT_FLOAT64,
        DT_INT8,
        DT_INT32,
        DT_INT64,
        DT_UINT8,
        DT_UINT32,
        DT_UINT64
    };
    
    enum ActivationType {
        LOGISTIC,
        RELU,
        RELU6,
        RELIE,
        LINEAR,
        RAMP,
        TANH,
        PLSE,
        REVLEAKY,
        LEAKY,
        ELU,
        LOGGY,
        STAIR,
        HARDTAN,
        LHTAN,
        SELU,
        GELU,
        SWISH,
        MISH,
        HARD_MISH,
        NORM_CHAN,
        NORM_CHAN_SOFTMAX,
        NORM_CHAN_SOFTMAX_MAXVAL
    };

    enum LayerType{
        CONVOLUTIONAL,
        DECONVOLUTIONAL,
        CONNECTED,
        MAXPOOL,
        LOCAL_AVGPOOL,
        SOFTMAX,
        DETECTION,
        DROPOUT,
        CROP,
        ROUTE,
        COST,
        NORMALIZATION,
        AVGPOOL,
        LOCAL,
        SHORTCUT,
        SCALE_CHANNELS,
        SAM,
        ACTIVE,
        RNN,
        GRU,
        LSTM,
        CONV_LSTM,
        HISTORY,
        CRNN,
        BATCHNORM,
        NETWORK,
        XNOR,
        REGION,
        YOLO,
        GAUSSIAN_YOLO,
        ISEG,
        REORG,
        REORG_OLD,
        UPSAMPLE,
        LOGXENT,
        L2NORM,
        EMPTY,
        BLANK,
        CONTRASTIVE
    };

    
    std::ostream& operator<< (std::ostream& out, const DataType& obj);
    std::ostream& operator<< (std::ostream& out, const DeviceType& obj);
} // namespace darknet

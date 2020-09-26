#pragma once


namespace darknet
{
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

} // namespace darknet

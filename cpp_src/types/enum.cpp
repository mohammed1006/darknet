#include "types/enum.hpp"


namespace darknet
{
    std::ostream& operator<< (std::ostream& out, const DataType& obj)
    {
        switch (obj)
        {
        case DataType::DT_FLOAT16:
            out << "float16";
            break;
        case DataType::DT_FLOAT32:
            out << "float32";
            break;
        case DataType::DT_FLOAT64:
            out << "float64";
            break;
        case DataType::DT_INT8:
            out << "int8";
            break;
        case DataType::DT_INT32:
            out << "int32";
            break;
        case DataType::DT_INT64:
            out << "int64";
            break;
        case DataType::DT_UINT8:
            out << "uint8";
            break;
        case DataType::DT_UINT32:
            out << "uint32";
            break;
        case DataType::DT_UINT64:
            out << "uint64";
            break;
        }
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const DeviceType& obj)
    {
        switch (obj)
        {
        case DeviceType::GPUDEVICE:
            out << "GPU";
            break;
        case DeviceType::CPUDEVICE:
            out << "CPU";
            break;
        }

        return out;
    }

} // namespace darknet

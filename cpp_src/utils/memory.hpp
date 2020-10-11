#pragma once

#include <stdlib.h>
#include "types/enum.hpp"

namespace darknet
{
namespace utils
{
    size_t num_bytes(DataType t)
    {
        switch (t)
        {
        case DataType::DT_UINT8:
        case DataType::DT_INT8:
            return 1;
        case DataType::DT_FLOAT16:
            return 2;
        case DataType::DT_FLOAT32:
        case DataType::DT_INT32:
        case DataType::DT_UINT32:
            return 4;
        case DataType::DT_FLOAT64:
        case DataType::DT_INT64:
        case DataType::DT_UINT64:
            return 8;
        }
        return 0;
    }

} // namespace utils
} // namespace darknet

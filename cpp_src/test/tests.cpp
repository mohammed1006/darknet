
#include "tensor/tensor.hpp"
#include "tensor/tensor_cpu.hpp"
#include "tensor/tensor_gpu.hpp"
#include "tensor/tensor_shape.hpp"
#include <gtest/gtest.h>
 
// TEST(DarkNetTest, CreatingObjects) {
//     std::string act = "logistic";
//     auto layer_1 = darknet::layer::Activation(nullptr, act);
// }

TEST(DarkentTensor, TestCreateCPUTensor)
{
    darknet::tensor::TensorShape shape({9, 3});
    
    darknet::tensor::Tensor<float, darknet::DeviceType::CPUDEVICE> matrix1();
    darknet::tensor::Tensor<float, darknet::DeviceType::CPUDEVICE> matrix2(shape);
}

TEST(DarkentTensor, TestCreateGPUTensor)
{
    darknet::tensor::TensorShape shape({9, 3});
    
    darknet::tensor::Tensor<float, darknet::DeviceType::GPUDEVICE> matrix1();
    darknet::tensor::Tensor<float, darknet::DeviceType::GPUDEVICE> matrix2(shape);
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
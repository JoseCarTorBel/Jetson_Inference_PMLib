
# JETSON INFERENCE PMLib

Performance evaluation and energy consumption of neural networks on [Nvidia Jetson TX2](https://developer.nvidia.com/embedded/jetson-tx2 "Nvidia Jetson TX2").

## Hardware Jetson TX2

Nvidia Jetson TX2 has CPU ARMv8: 2x Denver2 + 4x A57  + 1x GPU Pascal 256 cores + 8 GB  DDR4 RAM.

Consumption reading is done through INA3221 shunt type sensors integrated on the board.

The different operating modes are analyzed using the *nvpmodel* command that uses [Tegrastats](https://docs.nvidia.com/jetson/archives/l4t-archived/l4t-3231/index.html#page/Tegra%20Linux%20Driver%20Package%20Development%20Guide/AppendixTegraStats.html "Tegrastats").

|  Mode | Name  | ARMv8 Denver (Cores)  | Freq GHz |  ARMv8 A57  (Cores) | Freq. Ghz  | Pascal GPU GHz |
| :------------: | :------------: | :------------: | :------------: | :------------: | :------------: | :------------: |
| 0  | MAX N  |  2 |  2.0 | 4  | 2.0  |  1.30 |
| 1  |  MAX Q |  0  |   |  4 | 1.2  |  0.85 |
|  2 |  MAX P Core All | 2  | 1.4  | 4  | 1.4  |  1.12 |
|  3 |  MAX P ARM  |  0 |   | 4  |  2.0 | 1.12  |
|4| MAX P Denver | 1 | 2.0 | 1 | 2.0 | 1.12|


## Software

Nvidia TensorRT environment with PMLib for energy measurement. Then, the analyzed Convolutional Neural Network (CNN) have been AlexNet. GoogleNet-12, Inception-V3 and VGG-19 with ImageNet dataset.


## Results

## Conclusions

## References

- [Jetson Inference Toolking](https://github.com/dusty-nv/jetson-inference "Jetson Inference Toolking")
- [Power Measurement Library (PMLib)](https://github.com/mdolz/PMLib "Power Measurement Library (PMLib)")
- [Jetson Inference Batch](https://github.com/rockkingjy/Inference_jetson-inference-batch "Jetson Inference Batch")
-  [Nvidia Documentation](https://docs.nvidia.com/jetson/archives/l4t-archived/l4t-3231/index.html "Nvidia Documentation")





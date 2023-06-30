# GPTMR生成T型加减速脉冲

## 概述

t_shape_accel_decel示例工程演示了GPTMR生成T型加减速脉冲的功能。

## 硬件设置

请参考具体开发板{ref}`引脚描述 <lab_board_resource>`部分的描述。
连接GPTMR比较输出引脚至示波器。

## 运行现象

运行工程，示例执行GPTMR的生成T型加减速脉冲的功能，使用示波器观测输出比较的引脚。上电后会打印不同脉冲的重装载值，随后输出固定脉宽不同频率的脉冲，LED亮起代表生成完毕。串口终端输出如下信息：

```console

generate T_shape_accel_decel test
shape_table[0] :10000
shape_table[1] :9900
shape_table[2] :9800
shape_table[3] :9700
shape_table[4] :9600
shape_table[5] :9500
shape_table[6] :9400
shape_table[7] :9300
shape_table[8] :9200
shape_table[9] :9100
shape_table[10] :9000
shape_table[11] :8900
shape_table[12] :8800
shape_table[13] :8700
shape_table[14] :8600
shape_table[15] :8500
shape_table[16] :8400
shape_table[17] :8300
shape_table[18] :8200
shape_table[19] :8100
shape_table[20] :8000
shape_table[21] :7900
shape_table[22] :7800
....
```




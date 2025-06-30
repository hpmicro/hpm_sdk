HPM Math Middleware
==================

Overview
--------

The HPM Math Middleware is a comprehensive mathematical library designed specifically for HPMicro microcontrollers. It provides highly optimized mathematical functions for digital signal processing (DSP) and neural network operations, leveraging both hardware acceleration and software implementations.

Features
--------

DSP Functions
~~~~~~~~~~~~

The DSP library provides a wide range of optimized mathematical operations:

1. Basic Math Operations
   - Addition, subtraction, multiplication, division
   - Square root, power functions
   - Trigonometric functions
   - Vector operations

2. Complex Number Operations
   - Complex arithmetic
   - Complex conjugate
   - Complex magnitude and phase
   - Complex vector operations

3. Matrix Operations
   - Matrix addition and subtraction
   - Matrix multiplication
   - Matrix transpose
   - Matrix inversion
   - Matrix determinant

4. Filtering Functions
   - FIR filters
   - IIR filters
   - Convolution
   - Correlation
   - Digital filter design

5. Transform Functions
   - Fast Fourier Transform (FFT)
   - Discrete Fourier Transform (DFT)
   - Inverse FFT
   - Real FFT
   - Complex FFT

6. Statistics Functions
   - Mean, variance, standard deviation
   - Maximum and minimum
   - Root mean square
   - Statistical moments

7. Distance Calculations
   - Euclidean distance
   - Manhattan distance
   - Cosine similarity
   - Correlation distance

8. Sorting Algorithms
   - Quick sort
   - Merge sort
   - Bubble sort
   - Selection sort

9. Support Vector Machine (SVM)
   - Linear SVM
   - Polynomial SVM
   - RBF SVM
   - SVM prediction

10. Controller Functions
    - PID controller
    - State space controller
    - Digital controller design

Neural Network Functions
~~~~~~~~~~~~~~~~~~~~~~~

The neural network library provides optimized operations for machine learning applications:

1. Basic Neural Network Operations
   - Layer operations
   - Weight initialization
   - Bias operations
   - Activation functions

2. Convolution Operations
   - 2D convolution
   - Depthwise convolution
   - Pointwise convolution
   - Strided convolution
   - Dilation convolution

3. Pooling Operations
   - Max pooling
   - Average pooling
   - Global pooling
   - L2 pooling

4. Activation Functions
   - ReLU
   - Leaky ReLU
   - Sigmoid
   - Tanh
   - Softmax

5. Fully Connected Layers
   - Matrix multiplication
   - Bias addition
   - Weight updates
   - Backpropagation

6. Softmax Operations
   - Softmax activation
   - Log softmax
   - Cross entropy loss

7. Concatenation Operations
   - Channel concatenation
   - Depth concatenation
   - Spatial concatenation

8. TinyEngine Support
    - Optimized for embedded systems
    - Memory-efficient operations
    - Quantization support

Architecture
-----------

The middleware is organized into three main components:

1. DSP Library (nds_dsp)
   - Optimized for Andes DSP hardware
   - Supports both GCC and IAR toolchains
   - Provides SIMD operations
   - Hardware-accelerated functions
   - Low-latency operations

2. Neural Network Library (nds_nn)
   - Optimized neural network operations
   - Supports various neural network layers
   - Includes TinyEngine support
   - Quantization support
   - Memory-efficient implementations

3. Software DSP Library (sw_dsp)
   - Pure software implementation
   - Platform-independent operations
   - Fallback implementation
   - Reference implementation

Configuration
------------

The middleware can be configured through the following options:

1. Library Selection
   - HPM_EN_MATH_FFA_LIB: Enable FFA library
   - HPM_EN_MATH_DSP_LIB: Enable DSP library
   - HPM_EN_MATH_NN_LIB: Enable Neural Network library

2. DSP Configuration
   - HPM_MATH_DSP_STATISTICS
   - HPM_MATH_DSP_BASIC
   - HPM_MATH_DSP_COMPLEX
   - HPM_MATH_DSP_CONTROLLER
   - HPM_MATH_DSP_DISTANCE
   - HPM_MATH_DSP_FILTERING
   - HPM_MATH_DSP_MATRIX
   - HPM_MATH_DSP_SVM
   - HPM_MATH_DSP_TRANSFORM
   - HPM_MATH_DSP_UTILS
   - HPM_MATH_DSP_SORT

3. Neural Network Configuration
   - HPM_MATH_NN_ACTIVATION
   - HPM_MATH_NN_TINYENGINE
   - HPM_MATH_NN_BASIC
   - HPM_MATH_NN_CONCATENATION
   - HPM_MATH_NN_CONVOLUTION
   - HPM_MATH_NN_CONNECTED
   - HPM_MATH_NN_POOLING
   - HPM_MATH_NN_SOFTMAX
   - HPM_MATH_NN_UTIL

Usage
-----

To use the HPM Math Middleware:

1. Include the main header file:
   ```c
   #include "hpm_math.h"
   ```

2. Configure the required features through the configuration macros

3. Initialize the library (if required)

4. Call the desired mathematical functions

Example:
```c
#include "hpm_math.h"

void example_fft(void)
{
    float32_t input[1024];
    float32_t output[1024];

    // Initialize input data
    for (int i = 0; i < 1024; i++) {
        input[i] = sinf(2 * HPM_MATH_PI * i / 1024.0f);
    }

    // Perform FFT
    hpm_dsp_cfft_f32(input, output, 1024);
}
```

Build Configuration
-----------------

The middleware requires specific build configurations:

1. Toolchain Requirements
   - Must use nds or zcc toolchain
   - GCC or IAR compiler support
   - Andes DSP extension enabled

2. Linker Configuration
   - Link with DSP library (-ldsp)
   - Link with math library (-lm)
   - Link with neural network library (-lnn) when needed

3. Compiler Options
   - Enable DSP extension (-mext-dsp)
   - Configure optimization level
   - Set appropriate memory model

Performance Considerations
------------------------

1. Hardware Acceleration
   - Utilize DSP hardware when available
   - Optimize for SIMD operations
   - Minimize memory transfers

2. Memory Usage
   - Use appropriate data types
   - Consider memory alignment
   - Optimize buffer sizes

3. Computational Efficiency
   - Batch operations when possible
   - Use optimized algorithms
   - Consider fixed-point arithmetic

License
-------

BSD-3-Clause
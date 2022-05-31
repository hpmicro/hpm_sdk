# FFT Performance test
## Overview

The fft performance tests focus on the performance of DSP, ffa , software , fft and ifft transformations, and include the following computational units:
- DSP FFT-RADIX-2
- DSP FFT-RADIX-4
- DSP IFFT-RADIX-4
- DSP IFFT-RADIX-2
- DSP FFT-RADIX-2 Q31
- CPU Cooley-Tukey
- Q format and float32 interconversion

## Programming

The software generates several sine waves with different amplitudes and phases and superimposes them to generate a waveform. The ifft data and the fft transform data are compared to see if the ifft data and the fft transform data are the same.

## Performance Optimization

- segger turns on O3 optimization by default
- hpm_math is by default optimized for CPU Cooley-Tukey algorithm calculations, which increases space usage

## Running the example

The serial terminal outputs the name of the calculation, the number of samples, and the data of the amplitude and frequency characteristics, as well as the running time of each unit, and outputs PASS or ERROR NUM at the end. The specific performance is shown in the following table：

| Samples              | 64    | 128   | 256   | 512    | 1024   |
|------------------|-------|-------|-------|--------|--------|
| DSP radix-4 | 5900  | N     | 29869 | N      | 145366 |
| DSP radix-2| 9345  | 21220 | 47762 | 106211 | 234534 |
| DSP radix-2(Q31) | 4378  | 9581  | 21177 | 46126  | 101331 |
| FFAQ31           | 2729  | 4923  | 7769  | 18999  | N      |
| CPU(Cooley-Tukey)   | 14495 | 29171 | 61176 | 131084 | 283441 |
| DSP radix-4 | 6016  | N     | 30384 | N      | 147651 |
| DSP radix-2 | 9558  | 21632 | 48733 | 108296 | 238775 |
| DSP radix-2(Q31)| 4641  | 10301 | 22980 | 50645  | 111359 |
| FFAQ31           | 2687  | 4898  | 9152  | 18956  | N      |
| Q31 to Float     | 1208  | 2392  | 4757  | 9493   | 18965  |
| Float to Q31     | 5517  | 10958 | 21970 | 44022  | 88055  |


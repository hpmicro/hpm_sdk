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

The software generates several sine waves with different amplitudes and phases and superimposes them to generate a waveform. The ifft data and the fft transform data are compared to see if the ifft data and the fft transform data are the same. The sine wave is shown below:

- Amplitude 11, frequency 8
- Amplitude 19, frequency 17
- Amplitude 13, frequency 24

## Performance Optimization

- segger turns on O3 optimization by default
- hpm_math is by default optimized for CPU Cooley-Tukey algorithm calculations, which increases space usage

## Running the example

The serial terminal outputs the name of the calculation, the number of samples, and the data of the amplitude and frequency characteristics, as well as the running time of each unit, and outputs PASS or ERROR NUM at the end.

The serial port output is shown below:

```console

dsp fft radix-4 Total samples: 64.
total times:5813 tick.
------------------------------------
DC component amplitude:10.000006.
Frequency:8.000000, Amplitude:11.000008.
Frequency:17.000000, Amplitude:18.999988.
Frequency:24.000000, Amplitude:23.000000.
------------------------------------
dsp ifft radix-4 Total samples: 64.
total times:5961 tick.
------------------------------------
dsp fft radix-4 Total samples: 256.
total times:29671 tick.
------------------------------------
DC component amplitude:9.999996.
Frequency:8.000000, Amplitude:11.000002.
Frequency:17.000000, Amplitude:18.999998.
Frequency:24.000000, Amplitude:23.000003.
------------------------------------
dsp ifft radix-4 Total samples: 256.
total times:30220 tick.
------------------------------------
dsp fft radix-4 Total samples: 1024.
total times:144992 tick.
------------------------------------
DC component amplitude:9.999997.
Frequency:8.000000, Amplitude:11.000000.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:23.000000.
------------------------------------
dsp ifft radix-4 Total samples: 1024.
total times:147161 tick.
------------------------------------
**************************************
dsp fft radix-2 Total samples: 64.
total times:9280 tick.
------------------------------------
DC component amplitude:10.000006.
Frequency:8.000000, Amplitude:11.000008.
Frequency:17.000000, Amplitude:18.999988.
Frequency:24.000000, Amplitude:23.000000.
------------------------------------
dsp ifft radix-2 Total samples: 64.
total times:9544 tick.
------------------------------------
dsp fft radix-2 Total samples: 128.
total times:21119 tick.
------------------------------------
DC component amplitude:9.999998.
Frequency:8.000000, Amplitude:11.000001.
Frequency:17.000000, Amplitude:18.999996.
Frequency:24.000000, Amplitude:23.000003.
------------------------------------
dsp ifft radix-2 Total samples: 128.
total times:21575 tick.
------------------------------------
dsp fft radix-2 Total samples: 256.
total times:47643 tick.
------------------------------------
DC component amplitude:9.999996.
Frequency:8.000000, Amplitude:11.000002.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:23.000003.
------------------------------------
dsp ifft radix-2 Total samples: 256.
total times:48582 tick.
------------------------------------
dsp fft radix-2 Total samples: 512.
total times:106047 tick.
------------------------------------
DC component amplitude:9.999997.
Frequency:8.000000, Amplitude:11.000000.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:22.999998.
------------------------------------
dsp ifft radix-2 Total samples: 512.
total times:108005 tick.
------------------------------------
dsp fft radix-2 Total samples: 1024.
total times:234172 tick.
------------------------------------
DC component amplitude:9.999997.
Frequency:8.000000, Amplitude:11.000000.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:23.000000.
------------------------------------
dsp ifft radix-2 Total samples: 1024.
total times:237975 tick.
------------------------------------
**************************************
dsp fft q31 radix-2 Total samples: 64.
total times:3782 tick.
------------------------------------
DC component amplitude:10.000006.
Frequency:8.000000, Amplitude:11.000009.
Frequency:17.000000, Amplitude:18.999988.
Frequency:24.000000, Amplitude:23.000000.
------------------------------------
dsp ifft q31 radix-2 Total samples: 64.
total times:4220 tick.
------------------------------------
dsp fft q31 radix-2 Total samples: 128.
total times:8252 tick.
------------------------------------
DC component amplitude:9.999998.
Frequency:8.000000, Amplitude:11.000002.
Frequency:17.000000, Amplitude:18.999996.
Frequency:24.000000, Amplitude:23.000003.
------------------------------------
dsp ifft q31 radix-2 Total samples: 128.
total times:9274 tick.
------------------------------------
dsp fft q31 radix-2 Total samples: 256.
total times:17995 tick.
------------------------------------
DC component amplitude:9.999995.
Frequency:8.000000, Amplitude:11.000002.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:23.000005.
------------------------------------
dsp ifft q31 radix-2 Total samples: 256.
total times:20619 tick.
------------------------------------
dsp fft q31 radix-2 Total samples: 512.
total times:39039 tick.
------------------------------------
DC component amplitude:9.999995.
Frequency:8.000000, Amplitude:11.000001.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:23.000000.
------------------------------------
dsp ifft q31 radix-2 Total samples: 512.
total times:45407 tick.
------------------------------------
dsp fft q31 radix-2 Total samples: 1024.
total times:85195 tick.
------------------------------------
DC component amplitude:9.999995.
Frequency:8.000000, Amplitude:11.000002.
Frequency:17.000000, Amplitude:19.000003.
Frequency:24.000000, Amplitude:23.000001.
------------------------------------
dsp ifft q31 radix-2 Total samples: 1024.
total times:99603 tick.
------------------------------------
**************************************
Software fft  cooley tukey Total samples: 64.
total times:14493 tick.
------------------------------------
DC component amplitude:10.000006.
Frequency:8.000000, Amplitude:11.000014.
Frequency:17.000000, Amplitude:18.999998.
Frequency:24.000000, Amplitude:23.000019.
------------------------------------
Software fft  cooley tukey Total samples: 128.
total times:29009 tick.
------------------------------------
DC component amplitude:9.999998.
Frequency:8.000000, Amplitude:11.000004.
Frequency:17.000000, Amplitude:19.000003.
Frequency:24.000000, Amplitude:23.000015.
------------------------------------
Software fft  cooley tukey Total samples: 256.
total times:61116 tick.
------------------------------------
DC component amplitude:9.999996.
Frequency:8.000000, Amplitude:11.000000.
Frequency:17.000000, Amplitude:18.999998.
Frequency:24.000000, Amplitude:23.000000.
------------------------------------
Software fft  cooley tukey Total samples: 512.
total times:131520 tick.
------------------------------------
DC component amplitude:9.999997.
Frequency:8.000000, Amplitude:10.999999.
Frequency:17.000000, Amplitude:19.000001.
Frequency:24.000000, Amplitude:23.000000.
------------------------------------
Software fft  cooley tukey Total samples: 1024.
total times:285106 tick.
------------------------------------
DC component amplitude:9.999997.
Frequency:8.000000, Amplitude:10.999997.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:22.999996.
------------------------------------
**************************************
------------------------------------
convert data from float to q31, nums:128.
total times:3163 tick.
convert data from q31 to float, nums:128.
total times:1177 tick.
------------------------------------
------------------------------------
convert data from float to q31, nums:256.
total times:6225 tick.
convert data from q31 to float, nums:256.
total times:2325 tick.
------------------------------------
------------------------------------
convert data from float to q31, nums:512.
total times:12269 tick.
convert data from q31 to float, nums:512.
total times:4629 tick.
------------------------------------
------------------------------------
convert data from float to q31, nums:1024.
total times:24395 tick.
convert data from q31 to float, nums:1024.
total times:9237 tick.
------------------------------------
------------------------------------
convert data from float to q31, nums:2048.
total times:47949 tick.
convert data from q31 to float, nums:2048.
total times:18453 tick.
------------------------------------
PASS.

```

The performance is shown in the following table:

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


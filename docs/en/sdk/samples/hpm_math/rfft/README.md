# RFFT Performance test
## Overview

The rfft performance tests focus on the performance of DSP rfft.

## Programming

The software generates several sine waves with different amplitudes and phases for superposition, and then performs the rfft transform to find the amplitude and frequency data, and the sine wave is shown below:

- Amplitude 11, frequency 8
- Amplitude 19, frequency 17
- Amplitude 13, frequency 24

## Performance Optimization

- segger turns on O3 optimization by default

## Running the example

The serial terminal outputs the number of samples, the amplitude frequency data and the run time, and outputs PASS or ERROR NUM at the end.

The serial port output is shown below:

```console

Rfft demo: 64.
total times:5133 tick.
------------------------------------
DC component amplitude:5.000004.
Frequency:8.000000, Amplitude:11.000010.
Frequency:17.000000, Amplitude:18.999992.
Frequency:24.000000, Amplitude:13.000002.
------------------------------------
Rfft demo: 128.
total times:7956 tick.
------------------------------------
DC component amplitude:4.999997.
Frequency:8.000000, Amplitude:11.000002.
Frequency:17.000000, Amplitude:18.999996.
Frequency:24.000000, Amplitude:13.000004.
------------------------------------
Rfft demo: 256.
total times:25266 tick.
------------------------------------
DC component amplitude:4.999996.
Frequency:8.000000, Amplitude:11.000001.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:13.000004.
------------------------------------
Rfft demo: 512.
total times:37868 tick.
------------------------------------
DC component amplitude:4.999998.
Frequency:8.000000, Amplitude:11.000000.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:13.000000.
------------------------------------
Rfft demo: 1024.
total times:122305 tick.
------------------------------------
DC component amplitude:4.999998.
Frequency:8.000000, Amplitude:11.000000.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:13.000000.
------------------------------------
PASS.
----------------END--------------------

```

The performance is shown in the following table:

| Samples              | 64    | 128   | 256   | 512    | 1024   |
|------------------|-------|-------|-------|--------|--------|
| DSP rfft          | 5133  | 7956    | 25266 | 37868      | 122305 |


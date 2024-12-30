# SDM

## Overview

The SDM sample project demonstrates the function of data sampling and amplitude detection of the data stream output by the Sigma-Delta modulator of SDM peripherals.

## Board Setting

Please refer to [Pin Description](lab_board_resource) for specific board.
Different hardware setting are used depending on whether an AD sampling chip is on the development board.
- If there is no onboard AD sampling chip on the development board, please using an external sampling chip and connect the output CLK and DAT pins of the sampling chip to the CLK and DAT pins of the SDM on the board.
- If there is onboard AD sampling chip, simply connect the analog signal to the input interface of the sampling chip.

## Note
This example provides a conversion formula for input voltage and output value suitable for Sigma-Delta modulators such as AD7400 and NSI1306 (Full-scale ±320 mV). If you are using a different type of Sigma-Delta modulator, please check the conversion formula.


## Running the example

When running the project, the serial port will output the following information, allowing you to enter different numbers to execute different test functions:
```console
sdm example
*********************************************************************
*                                                                   *
*                         SDM Example Menu                          *
* Please enter one of following SDM function test(e.g. 1 or 2 ...): *
* 1 - SDM sample data in polling mode                               *
* 2 - SDM amplitude threshold check                                 *
* 3 - SDM sample data in interrupt mode                             *
* 4 - SDM sample data with sync signal                              *
*                                                                   *
*********************************************************************
```

### 1. SDM sample data in polling mode
SDM Polling Data Valid Flag and Reading Sample Data from FIFO.
```console
Selected option: 1

sdm sample data in polling mode.
203.57mV
203.17mV
203.31mV
203.14mV
203.01mV
203.28mV
203.17mV
203.05mV
203.19mV
203.21mV
203.02mV
203.19mV
203.43mV
203.17mV
203.29mV
203.24mV
```

### 2. SDM amplitude threshold check
SDM detects voltage over-limit events. The high voltage threshold for amplitude detection is set to SDM_AMPLITUDE_HIGH_THRESHOLD. When the input analog voltage increases from below this threshold to above it, an error interrupt is triggered.
```console
Selected option: 2

Set amplitude high threshold 240mV
sdm channel comparator detected amplitude over the high threshold
voltage: 253.75mV
```


### 3. SDM sample data in interrupt mode
SDM samples data from FIFO with the data ready flag. When the data in the FIFO reaches a preset threshold, SDM triggers a data ready interrupt. During the interrupt handler, data is read, and stop the sampling when the number of data read reaches TEST_DATA_COUNT.
```console
Selected option: 3

sdm sample data in interrupt mode.
203.39mV
203.08mV
202.96mV
202.87mV
202.85mV
202.86mV
202.86mV
202.84mV
202.81mV
202.88mV
203.01mV
203.23mV
203.08mV
202.97mV
202.92mV
202.97mV
```

### 4. SDM sample data with sync signal
In this case, GPTMR is used to output the synchronization signal, it connects to SDM sync input signal through TRGMMUX configuration. When the output signal of GPTMR rises, the synchronization event of SDM is triggered.
SDM samples data with synchronization signals. When a synchronization event occurs, SDM begins sampling data into the FIFO. Once the data in the FIFO reaches a preset threshold, SDM triggers a data ready interrupt to read the data, and the hardware clears the synchronization event flag, halting further sampling until the next synchronization event takes place.
```console
Selected option: 4

sdm sample data with sync signal. 1 time
203.88mV
203.84mV
203.90mV
203.87mV
203.91mV
203.92mV
203.85mV
203.85mV
sdm sample data with sync signal. 2 time
203.90mV
203.96mV
203.97mV
203.93mV
203.94mV
203.86mV
203.89mV
203.90mV
sdm sample data with sync signal. 3 time
203.94mV
204.03mV
203.97mV
203.91mV
203.89mV
203.87mV
203.93mV
203.97mV
sdm sample data with sync signal. 4 time
203.93mV
204.13mV
204.14mV
204.16mV
204.17mV
204.13mV
204.14mV
204.12mV
```

# HRPwm Output

## Overiew


**pwm** project configures several different types of waveforms and outputs them.

- Forced output
- Edge-aligned
- Center-aligned

## Configurations


- Dual Channel Oscilloscope
- Install the serial terminal, view [board information](lab_board_overiew), and configure the serial terminal parameters
- [HRPWM_P0 and HRPWM_P2 pins](lab_board_drv_hrpwm_pin) Check the information according to the board model


## Running the Demo

- After power on, the oscilloscope can observe P0, P1 output high level, low level, edge-aligned PWM and center-aligned PWM
- Compared with the reference PWM, the HRPWM output pulse width differs by `HRPWM_SET_IN_PWM_CLK` clock cycles
- Serial port printing information:

```console
hr pwm example


>> Test force HRPWM output on P0 and P2
Output high
Output low


>> Generate edge aligned waveform
Two waveforms will be generated, HRPWM P0 is the target waveform
whose duty cycle will be updated from 0 - 100 and back to 0; HRPWM P2 is a reference


>> Generate central aligned waveform
Two waveforms will be generated, HRPWM P0 is the target waveform
whose duty cycle will be updated from 0 - 100 and back to 0; HRPWM P2 is a reference
test done

```
```{note}
Output the PWM waveform once, and if you observe the waveform again, you need to run the program again.
```

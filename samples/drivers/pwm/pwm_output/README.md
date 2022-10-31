# Pwm Output

## Overiew


**pwm** project configures several different types of waveforms and outputs them.

- Forced output
- Edge-aligned
- Center-aligned
- Center-aligned complementary

## Configurations


- Dual Channel Oscilloscope
- Install the serial terminal, view [board information](lab_board_overiew), and configure the serial terminal parameters
- [PWM_P0 and PWM_P1 pins](lab_board_drv_pwm_pin) Check the information according to the board model


## Running the Demo

- After power on, the oscilloscope can observe P0, P1 output high level, low level, edge-aligned PWM, center-aligned PWM and center-aligned complementary PWM
- Serial port printing information:

```
pwm example

>> Test force PWM output on P0 and P1
Output high
Output low

>> Generate edge aligned waveform
Two waveforms will be generated, PWM P0 is the target waveform
whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

>> Generate central aligned waveform
Two waveforms will be generated, PWM P0 is the target waveform
whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

>> Generate central aligned waveform in pair
Two waveforms will be generated in pair, PWM P0 and P1 are target
waveforms whose duty cycle will be updated from 0 - 100 and back to 0
test done

```
```{note}
Output the PWM waveform once, and if you observe the waveform again, you need to run the program again.
```

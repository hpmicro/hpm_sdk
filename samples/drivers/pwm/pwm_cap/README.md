# Pwm Capture

## Overiew


The **PWM capture** project shows capturing a centrosymmetric PWM waveform by PWM CMP and get the duty cycle of PWM by calculation.

- PWM Capture
- Center-aligned

## Configurations

- Install the serial terminal, view [board information](lab_board_overiew), and configure the serial terminal parameters
- [PWM_P0 and PWM_P1 pins](lab_board_drv_pwm_pin) Check the information according to the board model
- Connect PWM_P0 and PWM_P1 pins


## Running the Demo

- PWM output duty cycle changes as a centrosymmetric waveform
- Capture waveform, calculate and output duty cycle
- Successfully capture the PWM signal, and the duty cycle is the same, the program returns PASS
- No waveform captured or the calculated duty cycle and output duty cycle do not match return FAIL
- Serial port printing information:

```
pwm capture example


>> Test PWM output on P4, PWM capture P5

Pwm output duty: 0.049995, Pwm cap duty: 0.054994.
Pwm output duty: 0.049995, Pwm cap duty: 0.049995.
Pwm output duty: 0.039996, Pwm cap duty: 0.044996.
Pwm output duty: 0.039996, Pwm cap duty: 0.039996.
Pwm output duty: 0.029997, Pwm cap duty: 0.034996.
Pwm output duty: 0.029997, Pwm cap duty: 0.029997.
Pwm output duty: 0.019998, Pwm cap duty: 0.024998.
Pwm output duty: 0.019998, Pwm cap duty: 0.019998.
Pwm output duty: 0.009999, Pwm cap duty: 0.014998.
Pwm output duty: 0.009999, Pwm cap duty: 0.009999.
PASS.
test done

```

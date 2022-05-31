# QEI

## Overiew

**QEI** project demonstrates the output of Hall timer data using the serial port after triggering the **QEI** interrupt

- Configuring the QEI Interrupts
- Read the QEI `U\V\W\TIMER` register

## Configurations

- Install the serial terminal, view [board information](lab_board_overiew), and configure the serial terminal parameters
- This program uses the **BLM57050-1000** brushless motor of "Leisai Intelligence", refer to the [Leisai Intelligence](https://leisai.com/) website for the specific parameters of the motor.
- Click [DRV-LV50A-MP1907 Motor Driver Board](lab_drv_lv50a_mp1907) section and configure

## Running the Demo

- Power on, open the serial terminal, turning the motor bearing will trigger a QEI interrupt and print the following information. The data changes with the speed and direction of rotation, please refer to the user manual for the exact meaning of the data.

```

> z: 0xffffff9c
> phase: 0xc200014d
> speed: 0xa0000000
> timer: 0xa8d08799

```

# Hall

## Overiew


**Hall** project demonstrates the output of Hall timer data using the serial port after triggering the **hall** interrupt

- Configuring the Hall Interrupts
- Read the Hall `U\V\W\TIMER` register

## Configurations

- Install the serial terminal, view [board information](lab_board_overiew), and configure the serial terminal parameters
- This program uses the **BLM57050-1000** brushless motor of "Leisai Intelligence", refer to the [Leisai Intelligence](https://leisai.com/) website for the specific parameters of the motor.
- Click [DRV-LV50A-MP1907 Motor Driver Board](lab_drv_lv50a_mp1907) section and configure

## Running the Demo

- Power on, open the serial terminal, turning the motor bearing will trigger a Hall interrupt and print the following information. The data changes with the speed and direction of rotation, please refer to the user manual for the exact meaning of the data.

```

> u: 0xe0000000
> v: 0x87b6d17c
> w: 0x46999a8
> timer: 0xe149af9

```

.. _dma_use_gptmr_event_transfer:

DMA Use GPTMR Event Transfer
========================================================

Overview
--------

- The sample shows the use of gptmr event triggers to request DMA transfers.

- the dst address for DMA transmission is the TOGGLE register of the GPIO port of the onboard LED,when the gptmr event occurs,the LED will toggle level and flash.

Board Setting
-------------

Please refer to  :ref:`Pin Description <board_resource>`  for specific board
- use the serial debug assistant and input the gptmr event request configure number

- when the gptmr event triggers to request DMA transfers is input_signal_toggle event, need to connect external PWM signal to GPTMR capture pin.

- The serial port is set to ``115200bps`` , ``1 stop bit`` , ``no parity bit``

Notice
------

-  When using the input_signal_toggle event trigger, it is recommended that the PWM input used be within 30Hz so that the LED flashing can be observed. If the input is higher than 30Hz, please use an oscilloscope to view the corresponding LED GPIO pin changes.

Running the example
-------------------

- use the serial debug assistant, press the enter key to print a prompt,select the corresponding event trigger according to the prompt, and the onboard LED will flash at different frequencies.

- If you need to change the blinking frequency of the LED in 0, 1, and 3 in the demo menu, you can change the default macros APP_BOARD_RELOAD_MS and APP_BOARD_CMP_MS in the code, which are 500ms and 100ms by default. 2 LED blinking effect is determined by the input PWM of the GPTMR capture pin.

- log as follows:


.. code-block:: console

   -----------------------------------------------------------------------------------
   *                                                                                 *
   *                   trigger dma transfer using gptmr event demo                   *
   *                                                                                 *
   *        0. use gptmr cmp0 event                                                  *
   *        1. use gptmr cmp1 event                                                  *
   *        2. use gptmr input_signal_toggle event                                   *
   *        3. use gptmr reload event                                                *
   *                                                                                 *
   *---------------------------------------------------------------------------------*


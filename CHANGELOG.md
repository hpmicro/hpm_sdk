# Change Log

## [0.11.0] - 2022-05-31:
Main changes since 0.10.0-hpm6360-er

### Changed:
  - readme: change the location of the starting document
  - components: change hal_adc_xx to hpm_adc_xx
  - drivers: update the I2C driver
  - cmake: change default rv_arch to rv32imac
  - cmake: modified in a zephyr-compatible way
  - drivers: uart: redefined struct with zephyr
  - cmake: change soc and board name
  - cmake: ses: support enable ext_dsp in project file.
  - samples/tinyusb/host: optimize the toggle rate of the blinking led
  - samples/tinyusb/device: replace mchtimer with board_timer for led_linking_taskk
  - samples/tinyusb/device: optimize the implement of led_blinking_task
  - soc: update IP header and soc header files

### Fixed:
  - samples: motor_ctrl: bldc_block: fix some error
  - middleware: hpm_math: fix andes toolchain compile error
  - middleware: hpm_math: fix libdsp.a error
  - middleware: hpm_math: add ext-dsp for SES
  - samples: openocd_algo: fix func_table placement.
  - boards: hpm6360evk: correct cpu frequency
  - boards: fix warning caused by code irregularities
  - boards: LED status is not the same between hpm6750mini rev-A and rev-B
  - boards: update board_led_write.

## Added:
  - driver: add spi_setup_dma_transfer() API
  - middleware: hpm_math: add software fft function
  - middleware: hpm_math: add ffa to hpm_math
  - samples: i2c: update the sample b2b
  - samples: hpm_math: add fft_performance demo
  - samples: add SPI DMA sample
  - samples: multicore coremark using debug console only
  - samples: 1588: add ptp v1 master/slave

## [0.10.0_hpm6300] - 2022-05-16:
Main changes since 0.10.0

### Changed:
  - ip register header file update
  - ses_proj: using generated complete cpu_regs_xml to replace general cpu registers xml
  - drivers: gpiom: move gpiom_gpio_t into hpm_gpiom_src.h
  - drivers: update the enet driver
  - samples: dram: change sdram test address in dma demo
  - samples: drivers: mbx: exclude flash targets for core0 as well.
  - samples: lwip: update ethernetif.c

### Fixed:
  - drivers: ptpc: update ptpc_init_timer_with_initial.
  - drivers: can: bug fix and update
  - soc: correct vector table inclusion and swi name
  - env.sh: fix HPM_SDK_BASE setting problem with msys.
  - samples: sha256_example: failed to run case 13 and 14 with gcc.

### Added:
  - soc: add HPM6360 support
  - drivers: add ffa, pllctlv2, dac
  - boards: add hpm6360evk support
  - components: add adcx module
  - components/enet_phy: add the RTL8201 driver
  - samples: drivers: dac, ffa
  - samples: rgb_red: add evkmini ver B support

## [0.10.0] - 2022-04-30:
Main changes since 0.9.0

### Changed:
  - drivers: gptmr: split irq enable/disable interface
  - drivers: can: 155 Update SJW setting in CAN driver
  - soc: HPM6750: rename safe stack symbol in link script
  - components: debug_console: wait uart tx flag on writing.

### Added:
  - drivers: enet: add 1588 support
  - drivers: can: Add TDC support
  - drivers: mchtmr: add counter init API.
  - drivers: dma: add dma_disable_channel() API
  - middleware: add wav decoder
  - samples: lwip: add ptp demos
  - samples: tinyusb: add msc, cdc demo
  - samples: audio_codec: add wav player demo
  - samples: add tinyuf2 initial support
  - samples: add initial uart dma rx idle demo

### Fixed:
  - soc: HPM6750: sysctl: fix cpu lp mode API.
  - drivers: uart: correct baudrate calculation.
  - drivers: usb: fix: no response in device mode when a USB cable is pluged out
  - boards: correct the pin setting related to USB

## [0.9.0] - 2022-04-01:
Main changes since 0.8.0

### Changed:
  - drivers: update WDG, UART, DMA, DMAMUX driver
  - drivers: enet remove enet_intf_selection
  - drivers: can: support configure bit timing via low-level bit timing parameters
  - drivers: optimize gpio driver
  - samples: exclude flash targets for mbx core1
  - samples: adjust SES project setting for coremark
  - samples: jpeg: update and integration jpeg decode samples
  - ses: use relpath for linker script in project file
  - ses: add HPM device name in generated project file
  - soc: HPM6750: add an interface selection api

### Added:
  - soc: HPM6750: add ram linker for core1
  - ses: support to use Andes toolchain
  - middleware: add hpm_math (replacing hpm_dsp)
  - samples: add lwip httpd
  - drivers: add section and alignment general instructions in hpm_common.h

### Fixed:
  - boards: hpm6750evk: fix bldc adc pinmux error
  - boards : hpm6750evk : pinmux : fix spi init pins error
  - samples: sdp: fix non-cacheable data initialization issue
  - samples: littlevgl: fix wrong picture patch in README

## [0.8.0] - 2022-03-04:
All changes since 0.7.3

### Changed:
  - rename middleware/sdmmc to middleware/hpm_sdmmc
  - place isr into .isr_vector section for irq non-vector mode
  - change csr functions to support llvm

### Fixed:
  - ses: fix issue in register xml
  - freertor: fix trap handler at non-vector mode
  - sdxc: fix software reset issue

### Added:
  - add sdk doc
  - add more sample doc
  - add multicore demo
  - i2c/uart: add dma support
  - add tensorflow lite for microcontroller

## [0.7.3] - 2022-02-23:
All changes since 0.7.2

### Changed:
  - freertos: change exception handling

## [0.7.2] - 2022-02-21:
All changes since 0.7.1

### Changed:
  - freertos: change freertos irq stack definition, passed in CMakeLists.txt, defined in linker
  - soc: hpm6750: add DISABLE_IRQ_PREEMPTIVE to check if it needs to enable irq preemption

### Fixed:
  - freertos: disable irq preemption

## [0.7.1] - 2022-02-13:

All changes since 0.7.0

### Changed:
  - drivers: adc12: update adc12_prd_config_t
  - samples: can: update case with interrupt and communication between two boards

### Fixed:
  - drivers: can: fix blocking transcation issue
  - samples: mbx: support run this example in SES
  - SES: startup: add fpu enable if abi is set to enable hw fp

### Added:
  - samples: multicore: add flash based multicore example
  - drivers: can: add apis to recvieve message for non-blocking use

## [0.7.0] - 2022-01-30:

All changes since 0.6.2

### Changed:
  - update default CPU frequency to 816MHz from 648MHz
  - update the ip headers
  - drivers: gpio: replace gpio_XXX_pins with gpio_XXX_port
  - drivers: gpio: remove pin level enum definition
  - drivers: i2s: driver update to remove mclk_div
  - drivers: ptpc: update driver
  - drivers: common: update get first set bit API
  - drivers: uart: split one enable with parameter into enable and disable interfaces
  - drivers: pwm: change name of output_channel config API
  - drivers: trgm: split separate API to enable/disable io output
  - soc: HPM6750: initialize noncacheable data in startup
  - soc: HPM6750: l1c: update fence.i call
  - samples: hello_world: add LED flashing
  - samples: littlevgl: remove lvgl example

### Fixed:
  - hpm6750evkmini: correct refresh cycle number of sdram
  - hpm6750evkmini: Fix the SDRAM memory range issue in flash_sdram_xip linker file
  - SES: remove no_relax option for linker, since segger has provide a patch to its ld for ses v6.10.
  - drivers: gpio: fix read pin issue:
  - drivers: usb: fix usb disconnection under linux environment
  - drivers: sdxc: Fixed the compatibility issue on different SD/eMMC cards in the sdcard_fatfs demo
  - drivers: gptmr: incorrect DMAEN configuration condition
  - drivers: gptmr: clear CNTRST bit after set.
  - SDK_DECLARE_EXT_ISR_M cannot work in the c++ file
  - FreeRTOS: fix ISR_STACK setting
  - components: touch: gt911: fix gpio write pin call

### Added:
  - SEG: add register definition file in generated embedded studio project
  - samples: drivers: gpiom: Add example to demonstrate gpiom's function
  - drivers: common: add macro to put data into noncacheable sections
  - middleware: integrate lwip

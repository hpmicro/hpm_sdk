SystemView
==========

SEGGER SystemView target sources, configurations, and RTOS patches.

https://www.segger.com/systemview  
https://wiki.segger.com/SystemView

## Included files

  * `SYSVIEW/`
    * `SEGGER_SYSVIEW.c`               - Main module for SystemView.
    * `SEGGER_SYSVIEW.h`               - Main header for SystemView.
    * `SEGGER_SYSVIEW_ConfDefaults.h`  - Default SystemView configuration.
    * `SEGGER_SYSVIEW_Int.h`           - Internal header for SystemView.
  * `SEGGER/`
    * `SEGGER.h`                       - Common header for SEGGER modules.
  * `Config/`
    * `Global.h`                       - Global configuration for SEGGER modules.
    * `SEGGER_SYSVIEW_Conf.h`          - SystemView configuration file.
  * `Sample/`
    * `COMM/`
      * `IP/`                          - Examples for SystemView IP Recorder implementation.
      * `UART/`                        - Examples for SystemView UART Recorder implementation.
      * `SEGGER_SYSVIEW_REC.*`         - Implementation of Recorder state machine.
    * `embOS/`
      * `SEGGER_SYSVIEW_embOS.*`       - SystemView RTOS Interface for embOS.
      * `SEGGER_SYSVIEW_embOS_Ultra.*` - SystemView RTOS Interface for embOS-Ultra.
      * `Config/`                      - Example SystemView Target Configuration with embOS.
    * `FreeRTOSV8/`
      * `SEGGER_SYSVIEW_FreeRTOS.*`    - SystemView RTOS Interface for FreeRTOS V8.
      * `Config/`                      - Example SystemView Target Configuration with FreeRTOS V8.
      * `Patch/`
        * `FreeRTOSV8.2.3_Core.patch`  - Patch for SystemView instrumentation of FreeRTOS V8.2.3.
    * `FreeRTOSV9/`
      * `SEGGER_SYSVIEW_FreeRTOS.*`    - SystemView RTOS Interface for FreeRTOS V9.
      * `Config/`                      - Example SystemView Target Configuration with FreeRTOS V9.
      * `Patch/`
        * `FreeRTOSV9_Core.patch`      - Patch for SystemView instrumentation of FreeRTOS V9.
    * `FreeRTOSV10/`
      * `SEGGER_SYSVIEW_FreeRTOS.*`    - SystemView RTOS Interface for FreeRTOS V10.
      * `Config/`                      - Example SystemView Target Configuration with FreeRTOS V10.
      * `Patch/`
        * `FreeRTOSV10_Core.patch`     - Patch for SystemView instrumentation of FreeRTOS V10.
        * `FreeRTOSV10_Amazon_Core.patch`  - Patch for SystemView instrumentation of Amazon FreeRTOS V10.
    * `FreeRTOSV10.4/`
      * `SEGGER_SYSVIEW_FreeRTOS.*`    - SystemView RTOS Interface for FreeRTOS V10.4.
      * `Config/`                      - Example SystemView Target Configuration with FreeRTOS V10.4.
      * `Patch/`
        * `FreeRTOS_10_4_3.patch`      - Patch for SystemView instrumentation of FreeRTOS V10.4.
    * `FreeRTOSV11/`
      * `SEGGER_SYSVIEW_FreeRTOS.*`    - SystemView RTOS Interface for FreeRTOS V11.
      * `Config/`                      - Example SystemView Target Configuration with FreeRTOS V11.
    * `MicriumOSKernel/`
      * `SEGGER_SYSVIEW_MicriumOSKernel.c` - SystemView RTOS Interface for Micrium OS.
      * `os_trace_events.h`            - SystemView RTOS Interface header for Micrium OS.
      * `Config/`                      - Example SystemView Target Configuration with Micrium OS.
    * `NuttX`
      * `SEGGER_SYSVIEW_NuttX.*`       - SystemView RTOS Interface for NuttX.
      * `Config/`                      - Example SystemView Target Configuration with NuttX.
      * `Patch/`
        * `nuttx-8.2.diff`             - Patch for SystemView instrumentation of NuttX.
    * `NoOS/`
      * `Config/`                      - Example SystemView Target Configuration with no RTOS.
    * `ThreadX`
      * `SEGGER_SYSVIEW_ThreadX.*`     - SystemView RTOS Interface for ThreadX.
      * `Config/`                      - Example SystemView Target Configuration with ThreadX.
    * `uCOS-II/`
      * `SEGGER_SYSVIEW_uCOSII.c`      - SystemView RTOS Interface for uC/OS-II.
      * `os_trace_events.h`            - SystemView RTOS Interface header for uC/OS-II.
      * `Config/`                      - Example SystemView Target Configuration with uC/OS-II.
    * `uCOS-III/`
      * `SEGGER_SYSVIEW_uCOSIII.c`     - SystemView RTOS Interface for uC/OS-III.
      * `os_trace_events.h`            - SystemView RTOS Interface header for uC/OS-III.
      * `Config/`                      - Example SystemView Target Configuration with uC/OS-III.

## Getting Started

Clone the SystemView repository (this one, SEGGERMicro/SystemView) and the RTT repository (SEGGERMicro/RTT) into your project.

Add include paths to:
 * `SystemView/Config`
 * `SystemView/SEGGER`
 * `SystemView/SYSVIEW`
 * `RTT/Config`
 * `RTT/RTT`

Add the sources to your project:
 * Core SystemView Sources:
   * `SystemView/SYSVIEW/*.c`
 * Core RTT Sources:
   * `RTT/RTT/*.c`
   * `RTT/RTT/*.s`
 * SystemView RTOS Interface:
   * e.g. `SystemView/Sample/embOS/SEGGER_SYSVIEW_embOS.c/.h`
 * SystemView Target Configuration:
   * e.g. `SystemView/Sample/embOS/Config/Cortex-M0/*.c`

Update `SystemView/Config/SEGGER_SYSVIEW_Conf.h` and `RTT/Config/SEGGER_RTT_Conf.h`
to match the target system.

*(Alternatively: Do not include `SystemView/Config` and `RTT/Config` and place a copy of `*_Conf.h` in the project.)*


# 扩展电路板

## **DRV-LV50A-MP1907**电机驱动板

(lab_drv_lv50a_mp1907)=

- 使用跳线端子连接J12、J7、J6到HW、HV、HW
- 使用跳线端子连接J10的VCC到3V3
- 将电机的U、V、W三相和J4端子按照正确的线序相连
- 将电机的信号线插入J14插座
- 将24V电源接入J3端子

- **NOTE:**
    1. **请注意电源正负极方向**
    2. **插接电路板时不要带电操作**
    3. **单独给电机驱动板上电，观察电流是否异常。如果异常请检查线路连接**
    4. **进行下一步前，请确保已经给电机驱动板断电**

- 核心板和电机驱动板的连接

	- **HPM6750EVK**核心板和**DRV-LV50A-MP1907**电机驱动板

		- 核心板的J8和电机驱动板的J12相连。**注意对照丝印，不要插反**
		- 下图是连接完成后的整体效果图，红色方框标出了可能需要操作的器件的位置：

		![image-1](../images/boards/extension/drv_lv50a_mp1907__oee.jpg "image-1")

## **HPM6750EVKMINI-TO-Motor**电机扩展板

(lab_board_hpm6750evkmini-to-motor-220530RevA)=

  - 需要配合[DRV-LV50A-MP1907电机驱动板](lab_drv_lv50a_mp1907)使用
  - j2和j3插件相连
  - **HPM6750EVKMINI-TO-Motor**电机扩展板、**HPM6750EVKMINI**核心板和**DRV-LV50A-MP1907**电机驱动板连接如下图所示：

 ![image-2](../images/boards/extension/hpm6750evkmini_to_motor_220530RevA.jpg "image-2")
## **LCD-800480W070TC** 显示屏

  (lab_lcd_800480w070tc)=
- 七寸显示屏480*800分辨率
- 支持触摸操作，GT911五点触摸控制器
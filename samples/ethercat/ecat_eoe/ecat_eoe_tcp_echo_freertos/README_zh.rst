.. _ecat_eoe_tcp_echo_freertos:

ECAT_EOE_TCP_ECHO_FREERTOS
===========================

1. 概述
---------

ECAT_EOE_TCP_ECHO_FREERTOS示例演示基于EtherCAT EoE功能实现的TCP Server功能。

该例程使用FreeRTOS作为实时操作系统并采用LWIP Socket接口处理网络通信。

EtherCAT EoE是EtherCAT协议的一项扩展功能，允许在EtherCAT主从设备之间传输标准以太网帧。

该例程程序支持对ESC的EEPROM数据进行初始化，能够简化更新ESC的EEPROM的步骤。

如果程序代码中包含由SSC Tool生成的EEPROM数据(eeprom.h), 会检查ESC的EEPROM中存储的数据并根据条件进行更新。

如果EEPROM中的EtherCAT Slave Controller Configuration Area(前8个Word)的数据checksum校验失败，则会使用eeprom.h中的数据初始化EEPROM。

如果EEPROM中的EtherCAT Slave Controller Configuration Area(前8个Word)的数据checksum校验成功，则会进一步校验EEPROM数据中的Product Code和Revision Code。

当Product Code不同或eeprom.h中的Revision Number大于当前已经存储的EEPROM数据的Revision Number时，则会使用eeprom.h中的数据初始化EEPROM。

该方法能够解决初次使用时EEPROM为空情况下checksum校验失败的问题，能够对EEPROM进行初始化。在程序升级阶段，新的程序代码包含的eeprom.h中的Revision Number大于当前已经存储的EEPROM数据的Revision Number时，会使用新程序中的eeprom.h初始化EEPROM，而无需通过主站工具如TwinCAT等去更新EEPROM。


2. 准备
---------

  请参照ECAT_IO的README
  软件版本：SSC Tool(SSC Version: 5.13.1; Config File Vers: 1.5.3.0)

3. 工程设置
---------------

  请参照ECAT_IO的README

  **注意：** 使用FLASH模拟EEPROM功能时，需要为FLASH_EEPROM内容分配合适的flash空间，避免与其他的flash内容产生冲突。

4. 生成从站协议栈代码
------------------------------

由于许可问题, HPM_SDK不提供EtherCAT从站协议栈代码(SSC), 用户须从倍福官网下载SSC Tool并生成从站协议栈代码

**注意：** <hpm_sdk>/samples/ethercat/ecat_eoe目录下的例程共享相同的SSC配置，生成SSC从站协议栈代码仅需执行一次，多个eoe例程即可共享。

4.1. 下载SSC Tool
~~~~~~~~~~~~~~~~~~~

  请参照ECAT_IO的README

4.2 SSC Tool中导入配置文件
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  配置文件路径为: <hpm_sdk>/samples/ethercat/ecat_eoe/SSC/Config/HPM_ECAT_EOE_Config.xml

4.3 SSC Tool生成协议栈代码
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1. 创建新工程，选择指定的配置文件

    .. image:: ../doc/eoe_ssc_create_project.png
       :alt:

  2. 导入应用xlsx：<hpm_sdk>/samples/ethercat/ecat_eoe/SSC/eoe.xlsx
  3. 指定输出目录，生成从站协议栈代码和设备描述文件ESI

4.4 SSC EOE协议栈代码修改
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  SSC的EOE协议栈在处理发送数据帧时，当数据未完整发出且mailbox出现满的情况时，处理逻辑存在问题，会导致剩余数据内容未被发出，需要修正如下：

  .. image:: ../doc/eoe_ssc_fix.png
     :alt:

  可通过打patch的方式完成上诉修改，在SSC目录下的命令行窗口执行：patch -d Src < ssc_eoe_fix.patch

  .. image:: ../doc/eoe_ssc_patch.png
     :alt:

  **注意** ：如果你的电脑上没有安装patch命令，你需要安装2.5.9版或更高版本的GNU patch。如果已安装，请跳过此步骤。从 `网站 <http://gnuwin32.sourceforge.net/packages/patch.htm>`_ 下载patch命令（当前版本为2.5.9），并将"patch.exe"的安装路径添加到系统环境变量中以便能在命令行窗口中执行patch命令。

5. TwinCAT工程设置
----------------------

  请参照ECAT_IO的README

5.1. 添加ESI文件
~~~~~~~~~~~~~~~~~~~~

  1. 请根据实际硬件端口数目修改SSC工具生成的ESI文件中的设备的属性(<Device Physics="YY">), 默认生成的属性为"YY"，表示实现了Port0和Port1两个端口。如实际实现了Port0, Port1和Port2三个端口，请手动修改属性为"YYY"。

  .. image:: ../doc/esc_device_physics_eoe.png
     :alt:

  2. 将ESI文件复制到TwinCAT的安装目录下(如: **C:/TwinCAT/3.1/Config/Io/EtherCAT**)。

5.2 创建工程
~~~~~~~~~~~~~~~~

  请参照ECAT_IO的README

5.3 软件配置
~~~~~~~~~~~~~~~~

  请参照ECAT_IO的README

5.4 扫描设备
~~~~~~~~~~~~~~~~

  请参照ECAT_IO的README

5.5 更新EEPROM
~~~~~~~~~~~~~~~~

  请选择 **eoe** 设备描述文件

  .. image:: ../doc/twincat_eeprom_update_eoe.png
     :alt:

  注意：当前例程已经支持EEPROM初始化功能，在初次使用时，会自动将SSC Tool生成的EEPROM数据写入ESC的EEPROM中。
  运行例程时无须使用TwinCAT去更新EEPROM，但通过TwinCAT等工具更新EEPROM的功能依然是可用的。

5.6 扫描到正确的时候后，进入OP模式(EoE功能只需在Init之上的状态即可运行)。

  .. image:: ../doc/eoe_freerun.png
     :alt:

6. EoE功能测试
----------------------

6.1 设置PC的IP地址
    PC的IP地址需要和从设备的IP地址位于同一子网中，否则无法通过EoE功能进行通信。

    .. image:: ../doc/eoe_set_pc_ip.png
       :alt:

6.2 TwinCAT中设置从站设备的IP地址
    1. 检查EOE功能设置

    .. image:: ../doc/twincat_eoe_check_setting.png
       :alt:

    2. 设置从设备的IP地址

    .. image:: ../doc/twincat_eoe_set_ip.png
       :alt:

    3. 点击 **TwinCAT->Restart TwinCAT (Config Mode)**，激活IP设置。

6.3 使用设置的从设备IP地址，测试ping功能

    .. image:: ../doc/eoe_ping_test.png
       :alt:

6.4 使用设置的从设备IP地址以及端口，测试TCP Server echo功能

    .. image:: doc/eoe_tcp_echo.png
          :alt:


7. 运行现象
---------------

当工程正确运行后, 串口终端会输出如下信息：

当需要初始化EEPROM数据时，EtherCAT初始化log如下：

.. code-block:: console

   EtherCAT EOE TCP echo sample with LwIP stack and Freertos.
   Init EEPROM content.
   Init EEPROM content successful.
   EEPROM loading successful, no checksum error.

当不需初始化EEPROM数据时，EtherCAT初始化log如下：

.. code-block:: console

   EtherCAT EOE TCP echo sample with LwIP stack and Freertos.
   No need to init EEPROM content.
   EEPROM loading successful, no checksum error.

正确运行的log如下：

.. code-block:: console

   LwIP Version: 2.1.2
   TCP Server Port: 5001
   EoE IP address setting:
   IPv4 Address: 192.168.100.10
   IPv4 Netmask: 255.255.255.0
   IPv4 Gateway: 192.168.100.2
   MAC         : 02:01:05:20:03:E9
   EoE IP address setting:
   IPv4 Address: 192.168.100.10
   IPv4 Netmask: 255.255.255.0
   IPv4 Gateway: 192.168.100.2
   MAC         : 02:01:05:20:03:E9
   new client connected from (192.168.100.2, 58163)




.. _opener_blinky:

OpENer Blinky
==========================

概述
---------

OpENer Blinky示例用于演示使用OpENer通过Ethernet/IP(EIP)协议实现远程IO的功能。

准备
---------

硬件
~~~~~~~~~~

  - 带Ethernet的开发板，请参考具体开发板  :ref:`引脚描述 <board_resource>`  查看Ethernet硬件

    - **根据开发板的硬件配置，在CMakeLists.txt中选择相应的RMII/RGMII，和phy芯片。**

  - 带网口的PC

      - TwinCAT3软件对PC网卡有适配问题, `部分支持的Intel网卡 <https://infosys.beckhoff.com/english.php?content=../content/1033/tc3_overview/9309844363.html&id=1489698440745036069>`_

  - 对于无板载SDRAM的开发板，需要注意build type为flash_sdram_xip时，需要先插入SDRAM卡

软件
~~~~~~~~~~

  - TwinCAT3.1(Build 4024.56)

TwinCAT工程设置
----------------------

创建工程
~~~~~~~~~~~~~~~~

  1. 打开TwinCAT软件，选择 **File->New->Project**

    .. image:: doc/Twincat_new_project_1.png
       :alt:

  2. 选择 **TwinCAT Project** , 命名后点击 **OK**

    .. image:: doc/Twincat_new_project_2.png
       :alt:

软件配置
~~~~~~~~~~~~~~~~

  1. 更新网卡驱动(第一次使用时需要)

    .. image:: doc/Twincat_ethernet_driver.png
       :alt:

    .. image:: doc/Twincat_ethernet_driver_2.png
       :alt:

  2. 时钟设置

    软件运行时可能遇到如下报错： 需要以管理员权限运行C：\TwinCAT\3.1\System\win8settick.bat
      - - -

      *Init4\RTime：Start Interrupt：Ticker started >> AdsWarning4115 （0x1013,RTIME：system clock setup failed）*
      - - -

      .. image:: doc/Twincat_set_tick.png
         :alt:

添加EIP Scaner
~~~~~~~~~~~~~~~~~~~~

  1. 点击Device, 右键添加新项

    .. image:: doc/add_new_interface.png
       :alt:

  2. 选择EIP Scaner

    .. image:: doc/seclet_new_interface.png
       :alt:

  3. 选择前边更新驱动后的网卡

    .. image:: doc/seclet_local_interface.png
       :alt:

  4. 配置ip地址

     注意此处的ip地址需要与开发板的ip地址在同一网段，且不能和PC本身的网卡地址一样。即pc,开发板和twincat软件中的三个ip地址需要都不一样。

    .. image:: doc/set_ip_address.png
       :alt:

  5. 重新进入配置模式，使上一步的ip配置生效

    .. image:: doc/reenter_config_mode.png
       :alt:

添加EDS文件
~~~~~~~~~~~~~~~~~~~~

  右键EIP Scaner，选择import EDS file，选择opener/opener_blinky_app.eds

    .. image:: doc/import_eds_file.png
       :alt:

扫描设备
~~~~~~~~~~~~~~~~~

  1. 右键EIP Scaner, 然后扫描

    .. image:: doc/scan.png
       :alt:

  2. 添加设备

    .. image:: doc/found_new_device.png
       :alt:

添加IO链接
~~~~~~~~~~~~~~~~~~

  .. image:: doc/add_io_connection.png
     :alt:

  如果此时列表中没有找到Exclusive Owner，那么可以手动指定一下eds文件。

  .. image:: doc/load_from_eds.jpg
     :alt:

IO操作
~~~~~~~~~~~~

  1. 输入IO, 按动评估板上的按键KEYA, Input/Key State数值发生变化

    .. image:: doc/keystate.png
       :alt:

  2. 输出IO,右键Led State, 选择Online, 点击Write写入数值。 写入1开发板LED亮起，写入0开发板LED熄灭。

    .. image:: doc/ledstate.png
       :alt:

运行现象
---------------

当工程正确运行后, 串口终端会输出如下信息, 输入输出IO状态与TwinCAT工程配置相对应：

.. code-block:: console

   This is Ethernet/IP blinky demo.
   LwIP Version: 2.1.2
   Enet phy init passed !

   Link Status: Down
   Link Status: Down

   Link Status: Up
   Link Speed:  100Mbps
   Link Duplex: Full duplex
   IPv4 Address: 192.168.100.10
   IPv4 Netmask: 255.255.255.0
   IPv4 Gateway: 192.168.100.1

   Mesaage receieved from host!


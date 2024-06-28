# ECAT_IO

## 1. 概述

ECAT_IO示例用于演示基于HPM6E80的ECAT外设和从站协议栈代码(SSC)实现ECAT数字IO从站的步骤。
- 输入IO(2bit): SW6
- 输出IO(2bit): LED8/LED3
- RUN LED: LED7
- ERR LED: LED6

## 2. 准备

### 2.1 硬件
  - HPM6E00EVK(RevA)
  - 带网口的PC
      - TwinCAT3软件对PC网卡有适配问题, [部分支持的Intel网卡](https://infosys.beckhoff.com/english.php?content=../content/1033/tc3_overview/9309844363.html&id=1489698440745036069)
### 2.2 软件
  - TwinCAT3.1(Build 4024.56)
  - SSC Tool(SSC Version: 5.13; Config File Vers: 1.5.3.0)
  ![](doc/ssc_version.png)

## 3. 工程设置

### 3.1 使用Flash模拟EEPROM
默认设置为使用flash模拟eeprom, 无需修改工程设置。
### 3.2 使用实际EEPROM
  - 如需使用实际的eeprom器件, 请在使用SSC Tool时将"Hardware"属性下的"ESC_EEPROM_EMULATION"和"CREATE_EEPROM_CONTENT"设置为0, 再生成从站协议栈代码。
  - 使用实际eeprom时, 在文件`CMakeLists.txt`中, 设置"set(CONFIG_EEPROM_EMULATION 0)"。

## 4. 生成从站协议栈代码

由于许可问题, HPM_SDK不提供EtherCAT从站协议栈代码(SSC), 用户须从倍福官网下载SSC Tool并生成从站协议栈代码。

### 4.1. 下载SSC Tool
  1. 进入[ETG下载网页](https://www.ethercat.org/en/downloads.html), 输入"SSC"过滤下载内容
    ![](doc/SSC_download_1.png)
    ![](doc/SSC_download_2.png)

      <u>**下载需要提供ETG会员账号和有效的Vendor ID**</u>

  2. 下载并安装SSC Tool

### 4.2 SSC Tool中导入配置文件
  1. 打开SSC Tool, 选择**Tool->Options**
    ![](doc/SSC_Tool_Options.png)
  2. 选择Configurations, 选择添加配置文件<hpm_sdk>/samples/ethercat/ecat_io/SSC/Config/HPM_ECAT_IO_Config.xml
    ![](doc/SSC_Import_Configuration.png)
### 4.3 SSC Tool中创建新的工程
  1. 点击**File->New**
    ![](doc/SSC_File_New.png)
  2. 点击**Custom**选项, 并在下拉列表中选择**HPMicro ECAT Digital IO <HPMicro>**
    ![](doc/SSC_New_Project_Custom.png)
  3. 保存工程文件, 指定保存路径
    ![](doc/SSC_Project_Save.png)
  4. 导入应用
    - 选择**Tool->Application->Import**, 导入文件<hpm_sdk>/samples/ethercat/ecat_io/SSC/digital_io.xlsx
    ![](doc/SSC_Application_Import_1.png)
    ![](doc/SSC_Application_Import_2.png)
    - 点击**OK**
    ![](doc/SSC_Application_Import_3.png)
### 4.4 生成协议栈代码
  1. 点击**Project->Create new Slave Files**
    ![](doc/SSC_Create_Slave_1.png)
  2. 选择输出路径, 须设置Src输出路径为ecat_io/SSC/Src路径, 或者设置其他路径将生成的生成的Src复制到ecat_io/SSC/Src目录下
  3. 点击Start生成从站代码和ESI文件
    ![](doc/SSC_Create_Slave_2.png)

## 5. TwinCAT工程设置

### 5.1. 添加ESI文件
  将SSC工具生成的ESI文件(**ECAT-IO.xml**)复制到TwinCAT的安装目录下(如: **C:\TwinCAT\3.1\Config\Io\EtherCAT**)。

### 5.2 创建工程
  1. 打开TwinCAT软件，选择**File->New->Project**
    ![](doc/Twincat_new_project_1.png)
  2. 选择**TwinCAT Project**, 命名后点击**OK**
    ![](doc/Twincat_new_project_2.png)

### 5.3 软件配置
  1. 更新网卡驱动(第一次使用时需要)
    ![](doc/Twincat_ethernet_driver.png)
    ![](doc/Twincat_ethernet_driver_2.png)
  2. 时钟设置
    软件运行时可能遇到如下报错： 需要以管理员权限运行C：\TwinCAT\3.1\System\win8settick.bat
      - - -
      *Init4\RTime：Start Interrupt：Ticker started >> AdsWarning4115 （0x1013,RTIME：system clock setup failed）*
      - - -
      ![](doc/Twincat_set_tick.png)

### 5.4 扫描设备
  1. 点击Device, 右键Scan
    ![](doc/Twincat_scan.png)
  2. 选择对应的网络设备
    ![](doc/Twincat_scan_2.png)

### 5.5 更新EEPROM
  1. 双击扫描出的设备, 选择**Online**, 选择**Box**, 右键选择**EEPROM Update**。

      ***对于EEPROM内容为空的情况, 扫描设备时无法查询到匹配的Box, 此时需要更新EEPROM内容。***

      ***在EEPROM内容为空的情况下, ESC上电加载EEPROM数据时会出现checksum错误, 该错误会造成PDI不工作, 此时EtherCAT通信是可能的, 可以通过主站更新EEPROM内容。***

      ![](doc/Twincat_eeprom_update.png)
  2. 选择对应的ESI文件, 点击**OK**, 等待更新完成
      ![](doc/Twincat_eeprom_update_3.png)

  3. 重新上电评估板, 删除已扫描出的设备重新扫描后, 运行设备
      ![](doc/Twincat_device_remove.png)
      ![](doc/Twincat_scan_device_2.png)

### 5.6 IO操作
  1. 输入IO, 拨动评估板上的拨码SW6, InputCounter数值发生变化
    ![](doc/Twincat_input.png)
  2. 输出IO, 双击OutputCounter, 选择Online, 点击Write写入数值。 评估板LED9和LED3发生变化
    ![](doc/Twincat_output.png)
  3. LED7为ECAT的RUN_LED, LED6为ECAT的ERROR_LED

### 5.7 DC设置
  1. 双击Box, 选择DC, 点击Advanced Settings
      ![](doc/Twincat_DC_1.png)
  2. 设置模式为DC-Synchron并设置Shift Time(通常为Sync Unit Cycle时间的20%~30%)
      ![](doc/Twincat_DC_2.png)
  3. 双击Device, 选择EtherCAT, 点击Advanced Settings进行配置
      ![](doc/Twincat_DC_3.png)
  4. 选择Distributed Clock, 勾选DC in use, 之后点击确定
      ![](doc/Twincat_DC_4.png)
  5. 点击Restart TwinCAT
      ![](doc/Twincat_DC_5.png)


## 6. 运行现象

当工程正确运行后, 串口终端会输出如下信息, 输入输出IO状态与TwinCAT工程配置相对应：
```console
EtherCAT IO sample
EEPROM loading with checksum error.
EtherCAT communication is possible even if the EEPROM is blank(checksum error),
but PDI not operational, please update eeprom  context.
```
或是
```console
EtherCAT IO sample
EEPROM loading successful, no checksum error.
```



























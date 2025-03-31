.. _lwip_ptp_v2_slave:

lwip_ptp_v2_slave
==================================

概述
------

本示例展示PTP V2 Slave的功能

- Master与Slave通过PTP协议通讯，从而实现Slave端的时钟同步

硬件设置
------------

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

* 使用以太网线缆连接开发板RGMII或者RMII端口和另一个块开发板的RGMII或RMII端口

工程配置
------------

- 以太网端口配置：参考 :ref:`以太网通用工程设置 <ethernet_port_configurations_zh>`

- 以太网DHCP配置

  - lwIP sample:  在`CMakeLists.txt`中，支持如下配置:

    - sdk_compile_definitions(-DLWIP_DHCP=0): 禁用DHCP功能

    - sdk_compile_definitions(-DLWIP_DHCP=1): 启用DHCP功能

运行示例
------------

* 编译下载程序

* 串口终端显示如下信息：


  .. code-block:: console

     This is an ethernet demo: PTP V2 Slave
     LwIP Version: 2.1.2
     Enet phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     IPv4 Address: 192.168.100.11
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.100.1
     (D 1651074123.993909840) event POWER UP
     (D 1651074123.997502600) state PTP_INITIALIZING
     (D 1651074124.001787280) manufacturerIdentity: PTPd;2.0.1
     (D 1651074124.006950560) netShutdown
     (D 1651074124.010272080) netInit
     (D 1651074124.013374440) initData
     (D 1651074124.016455720) initTimer
     (D 1651074124.019601720) initClock
     (D 1651074124.022784800) state PTP_SLAVE
     (D 1651074124.026452480) state PTP_SLAVE
     (D 1651074124.030542480) event MASTER_CLOCK_CHANGED
     (D 1651074124.035176560) state PTP_UNCALIBRATED
     (D 1651074145.022257800) setTime: resetting system clock to 1651074145s 22257480ns
     (D 1651074145.029618800) initClock
     (D 1651074145.032771360) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074145.041164960) updateClock: offset from master:             -20s  -640741560ns
     (D 1651074145.049030320) updateClock: observed drift:                   0
     (D 1651074146.022215720) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074146.030607960) updateClock: offset from master:               0s      -62740ns
     (D 1651074146.038472200) updateClock: observed drift:               -4182
     (D 1651074147.022173020) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074147.030572400) updateClock: offset from master:               0s      -74250ns
     (D 1651074147.038430060) updateClock: observed drift:               -9132
     (D 1651074148.022165680) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074148.030558440) updateClock: offset from master:               0s      -86155ns
     (D 1651074148.038422620) updateClock: observed drift:              -14875
     (D 1651074149.022161780) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074149.030566320) updateClock: offset from master:               0s      -92408ns
     (D 1651074149.038425520) updateClock: observed drift:              -21035
     (D 1651074150.022157260) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074150.030543480) updateClock: offset from master:               0s      -91184ns
     (D 1651074150.038401400) updateClock: observed drift:              -27113
     (D 1651074151.022188300) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074151.030574040) updateClock: offset from master:               0s      -83492ns
     (D 1651074151.038432060) updateClock: observed drift:              -32679
     (D 1651074152.022195680) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074152.030590380) updateClock: offset from master:               0s      -71696ns
     (D 1651074152.038448800) updateClock: observed drift:              -37458
     (D 1651074153.022204460) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074153.030589580) updateClock: offset from master:               0s      -58398ns
     (D 1651074153.038446540) updateClock: observed drift:              -41351
     (D 1651074154.022225420) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074154.030617660) updateClock: offset from master:               0s      -45739ns
     (D 1651074154.038475240) updateClock: observed drift:              -44400
     (D 1651074155.022231280) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074155.030622340) updateClock: offset from master:               0s      -35019ns
     (D 1651074155.038479320) updateClock: observed drift:              -46734
     (D 1651074156.022216200) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074156.030599680) updateClock: offset from master:               0s      -26790ns
     (D 1651074156.038456660) updateClock: observed drift:              -48520
     (D 1651074157.022209360) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074157.030596340) updateClock: offset from master:               0s      -20965ns
     (D 1651074157.038453780) updateClock: observed drift:              -49917
     (D 1651074158.022215400) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074158.030610660) updateClock: offset from master:               0s      -17102ns
     (D 1651074158.038469560) updateClock: observed drift:              -51057
     (D 1651074159.022216480) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074159.030613420) updateClock: offset from master:               0s      -14591ns
     (D 1651074159.038481220) updateClock: observed drift:              -52029
     (D 1651074160.022239220) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074160.030635280) updateClock: offset from master:               0s      -12916ns
     (D 1651074160.038492220) updateClock: observed drift:              -52890
     (D 1651074161.022236600) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074161.030630460) updateClock: offset from master:               0s      -11638ns
     (D 1651074161.038487360) updateClock: observed drift:              -53665
     (D 1651074162.022237960) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074162.030627780) updateClock: offset from master:               0s      -10479ns
     (D 1651074162.038484740) updateClock: observed drift:              -54363
     (D 1651074163.022225960) updateClock: one-way delay averaged (P2P):           0s           0ns
     (D 1651074163.030616940) updateClock: offset from master:               0s       -9329ns
     (D 1651074163.038473980) updateClock: observed drift:              -54984
     (D 1651074163.045091880) event MASTER_CLOCK_SELECTED
     (D 1651074163.049810980) state PTP_SLAVE
     (D 1651074164.022247500) updateClock: one-way delay averaged (P2P):           0s         580ns
     (D 1651074164.030643680) updateClock: offset from master:               0s       -8425ns
     (D 1651074164.038501340) updateClock: observed drift:              -55545
     (D 1651074165.022273420) updateClock: one-way delay averaged (P2P):           0s         580ns
     (D 1651074165.030673040) updateClock: offset from master:               0s       -7312ns
     (D 1651074165.038530700) updateClock: observed drift:              -56032
     (D 1651074166.022266140) updateClock: one-way delay averaged (P2P):           0s         580ns
     (D 1651074166.030668040) updateClock: offset from master:               0s       -6126ns
     (D 1651074166.038528020) updateClock: observed drift:              -56440
     (D 1651074167.022257640) updateClock: one-way delay averaged (P2P):           0s         580ns
     (D 1651074167.030654260) updateClock: offset from master:               0s       -4993ns
     (D 1651074167.038514120) updateClock: observed drift:              -56772
     (D 1651074168.022248260) updateClock: one-way delay averaged (P2P):           0s         575ns
     (D 1651074168.030650640) updateClock: offset from master:               0s       -4004ns
     (D 1651074168.038507940) updateClock: observed drift:              -57038
     (D 1651074169.022241420) updateClock: one-way delay averaged (P2P):           0s         572ns
     (D 1651074169.030641960) updateClock: offset from master:               0s       -3188ns
     (D 1651074169.038500660) updateClock: observed drift:              -57250
     (D 1651074170.022244560) updateClock: one-way delay averaged (P2P):           0s         572ns
     (D 1651074170.030641640) updateClock: offset from master:               0s       -2560ns
     (D 1651074170.038498540) updateClock: observed drift:              -57420
     (D 1651074171.022264540) updateClock: one-way delay averaged (P2P):           0s         572ns
     (D 1651074171.030662980) updateClock: offset from master:               0s       -2096ns
     (D 1651074171.038520240) updateClock: observed drift:              -57559
     (D 1651074172.022243100) updateClock: one-way delay averaged (P2P):           0s         572ns
     (D 1651074172.030629780) updateClock: offset from master:               0s       -1754ns
     (D 1651074172.038486880) updateClock: observed drift:              -57675
     (D 1651074173.022254420) updateClock: one-way delay averaged (P2P):           0s         571ns
     (D 1651074173.030659540) updateClock: offset from master:               0s       -1503ns
     (D 1651074173.038516640) updateClock: observed drift:              -57775
     (D 1651074174.022237580) updateClock: one-way delay averaged (P2P):           0s         571ns
     (D 1651074174.030639900) updateClock: offset from master:               0s       -1307ns
     (D 1651074174.038497120) updateClock: observed drift:              -57862
     (D 1651074175.022241580) updateClock: one-way delay averaged (P2P):           0s         571ns
     (D 1651074175.030633620) updateClock: offset from master:               0s       -1139ns
     (D 1651074175.038490920) updateClock: observed drift:              -57937
     (D 1651074176.022270480) updateClock: one-way delay averaged (P2P):           0s         568ns
     (D 1651074176.030673920) updateClock: offset from master:               0s        -983ns
     (D 1651074176.038530780) updateClock: observed drift:              -58002
     (D 1651074177.022269220) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074177.030671580) updateClock: offset from master:               0s        -845ns
     (D 1651074177.038528440) updateClock: observed drift:              -58058
     (D 1651074178.022242680) updateClock: one-way delay averaged (P2P):           0s         566ns
     (D 1651074178.030641980) updateClock: offset from master:               0s        -706ns
     (D 1651074178.038500320) updateClock: observed drift:              -58105
     (D 1651074179.022261020) updateClock: one-way delay averaged (P2P):           0s         566ns
     (D 1651074179.030658200) updateClock: offset from master:               0s        -586ns
     (D 1651074179.038515820) updateClock: observed drift:              -58144
     (D 1651074180.022256820) updateClock: one-way delay averaged (P2P):           0s         565ns
     (D 1651074180.030653440) updateClock: offset from master:               0s        -475ns
     (D 1651074180.038510900) updateClock: observed drift:              -58175
     (D 1651074181.022250000) updateClock: one-way delay averaged (P2P):           0s         565ns
     (D 1651074181.030641080) updateClock: offset from master:               0s        -390ns
     (D 1651074181.038498260) updateClock: observed drift:              -58201
     (D 1651074182.022252600) updateClock: one-way delay averaged (P2P):           0s         566ns
     (D 1651074182.030650880) updateClock: offset from master:               0s        -328ns
     (D 1651074182.038507900) updateClock: observed drift:              -58222
     (D 1651074183.022269260) updateClock: one-way delay averaged (P2P):           0s         566ns
     (D 1651074183.030672940) updateClock: offset from master:               0s        -267ns
     (D 1651074183.038529720) updateClock: observed drift:              -58239
     (D 1651074184.022236380) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074184.030636200) updateClock: offset from master:               0s        -227ns
     (D 1651074184.038493180) updateClock: observed drift:              -58254
     (D 1651074185.022234240) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074185.030627760) updateClock: offset from master:               0s        -187ns
     (D 1651074185.038484580) updateClock: observed drift:              -58266
     (D 1651074186.022238520) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074186.030635400) updateClock: offset from master:               0s        -147ns
     (D 1651074186.038492340) updateClock: observed drift:              -58275
     (D 1651074187.022271020) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074187.030664600) updateClock: offset from master:               0s        -127ns
     (D 1651074187.038522300) updateClock: observed drift:              -58283
     (D 1651074188.022245000) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074188.030645420) updateClock: offset from master:               0s         -97ns
     (D 1651074188.038502200) updateClock: observed drift:              -58289
     (D 1651074189.022235340) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074189.030629200) updateClock: offset from master:               0s         -72ns
     (D 1651074189.038486060) updateClock: observed drift:              -58293
     (D 1651074190.022265520) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074190.030664320) updateClock: offset from master:               0s         -50ns
     (D 1651074190.038521340) updateClock: observed drift:              -58296
     (D 1651074191.022252920) updateClock: one-way delay averaged (P2P):           0s         566ns
     (D 1651074191.030654880) updateClock: offset from master:               0s         -38ns
     (D 1651074191.038512460) updateClock: observed drift:              -58298
     (D 1651074192.022234080) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074192.030633120) updateClock: offset from master:               0s         -22ns
     (D 1651074192.038489860) updateClock: observed drift:              -58299
     (D 1651074193.022255460) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074193.030654560) updateClock: offset from master:               0s          -5ns
     (D 1651074193.038511360) updateClock: observed drift:              -58299
     (D 1651074194.022275560) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074194.030672260) updateClock: offset from master:               0s           4ns
     (D 1651074194.038528920) updateClock: observed drift:              -58299
     (D 1651074195.022240660) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074195.030633860) updateClock: offset from master:               0s          19ns
     (D 1651074195.038490760) updateClock: observed drift:              -58298
     (D 1651074196.022239180) updateClock: one-way delay averaged (P2P):           0s         568ns
     (D 1651074196.030640640) updateClock: offset from master:               0s          15ns
     (D 1651074196.038497460) updateClock: observed drift:              -58297
     (D 1651074197.022277320) updateClock: one-way delay averaged (P2P):           0s         568ns
     (D 1651074197.030676040) updateClock: offset from master:               0s          24ns
     (D 1651074197.038533180) updateClock: observed drift:              -58296
     (D 1651074198.022269720) updateClock: one-way delay averaged (P2P):           0s         568ns
     (D 1651074198.030669140) updateClock: offset from master:               0s          28ns
     (D 1651074198.038526320) updateClock: observed drift:              -58295
     (D 1651074199.022242280) updateClock: one-way delay averaged (P2P):           0s         568ns
     (D 1651074199.030635460) updateClock: offset from master:               0s          30ns
     (D 1651074199.038498420) updateClock: observed drift:              -58293
     (D 1651074200.022241120) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074200.030644840) updateClock: offset from master:               0s          21ns
     (D 1651074200.038505340) updateClock: observed drift:              -58292
     (D 1651074201.022265680) updateClock: one-way delay averaged (P2P):           0s         567ns
     (D 1651074201.030662860) updateClock: offset from master:               0s          27ns
     (D 1651074201.038520000) updateClock: observed drift:              -58291



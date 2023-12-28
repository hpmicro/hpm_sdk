# lwip_ptp_v1_slave

## 概述

本示例展示PTP V1 Slave的功能

- Master与Slave通过PTP协议通讯，从而实现Slave端的时钟同步

## 硬件设置

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
* 使用以太网线缆连接开发板RGMII或者RMII端口和另一个块开发板的RGMII或RMII端口

## 工程配置

- 以太网端口配置：参考 [以太网通用工程设置](../../../doc/Ethernet_Common_Project_Settings_zh.md)
- 以太网DHCP配置
    - lwIP sample:  在`CMakeLists.txt`中，支持如下配置:
      - sdk_compile_definitions(-DLWIP_DHCP=0): 禁用DHCP功能
      - sdk_compile_definitions(-DLWIP_DHCP=1): 启用DHCP功能


## 运行示例

* 编译下载程序
* 串口终端显示如下信息：

  ```console
  This is an ethernet demo: PTP V1 Slave
  LwIP Version: 2.1.2
  Enet phy init passed !
  Static IP: 192.168.100.11
  Netmask  : 255.255.255.0
  Gateway  : 192.168.100.1
  (D 1651074120.007099320) event POWER UP
  (D 1651074120.010684080) initClock
  (D 1651074120.013845920) state PTP_INITIALIZING
  (D 1651074120.018134040) manufacturerIdentity: PTPd;1.0.1
  (D 1651074120.023292440) netShutdown
  igmp_leavegroup: attempt to leave non-multicast address(D 1651074120.031424000) netInit
  (D 1651074120.034514640) initData
  (D 1651074120.037589240) initTimer
  (D 1651074120.040733520) initClock
  (D 1651074120.043891800) sync message interval: 1000
  (D 1651074120.048611720) clock identifier: DFLT
  (D 1651074120.052897280) 256*log2(clock variance): -4000
  (D 1651074120.057968040) clock stratum: 255
  (D 1651074120.061900040) clock preferred?: no
  (D 1651074120.066008000) bound interface name: 
  (D 1651074120.070288760) communication technology: 1
  (D 1651074120.075004240) uuid: 04:00:00:00:e0:d8
  (D 1651074120.079382440) PTP subdomain name: _DFLT
  (D 1651074120.083924240) subdomain address: 224.0.1.129
  (D 1651074120.088904400) event port address: 3f 1
  (D 1651074120.093360000) general port address: 40 1
  (D 1651074120.097992080) initClock
  (D 1651074120.101136200) state PTP_LISTENING
  (D 1651074120.105154240) initClock
  (D 1651074120.108295480) state PTP_SLAVE
  (D 1651074123.300347360) addForeign: new record (0,1) 1 1 cc:00:00:00:80:85
(D 1651074123.307125280) event MASTER_CLOCK_CHANGED
  (D 1651074123.311744080) state PTP_UNCALIBRATED
(D 1651074123.316015160) toState: Q = 0, R = 6
  Link Status: Up
  Link Speed:  1000Mbps
  Link Duplex: Full duplex
(D 1651074124.299456640) updateOffset
  (D 1651074124.302869960) updateClock seconds
  (D 1651074589.017716080) setTime: resetting system clock to 1651074589s 17715780ns
  (D 1651074589.025053600) initClock
  (D 1651074589.028204720) one-way delay:           0s          0ns
(D 1651074589.034057680) offset from master:      -464s -710834980ns
  (D 1651074589.040172080) observed drift:          0
  (D 1651074590.010209400) updateOffset
  (D 1651074590.013647800) one-way delay:           0s          0ns
  (D 1651074590.019500240) offset from master:      0s     -54040ns
  (D 1651074590.025351300) observed drift:          -3602
  (D 1651074591.010164580) updateOffset
  (D 1651074591.013587700) one-way delay:           0s          0ns
(D 1651074591.019436680) offset from master:      0s     -64920ns
  (D 1651074591.025284480) observed drift:          -7930
  (D 1651074592.010144460) updateOffset
  (D 1651074592.013564160) one-way delay:           0s          0ns
  (D 1651074592.019413580) offset from master:      0s     -76320ns
(D 1651074592.025261680) observed drift:          -13018
  (D 1651074593.010137760) updateOffset
  (D 1651074593.013557540) one-way delay:           0s          0ns
  (D 1651074593.019408440) offset from master:      0s     -82580ns
  (D 1651074593.025257840) observed drift:          -18523
(D 1651074594.010138260) handleSync: Q = 0, R = 4
  (D 1651074594.014617580) updateOffset
  (D 1651074594.018027180) one-way delay:           0s          0ns
  (D 1651074594.023875160) offset from master:      0s     -81950ns
  (D 1651074594.029723260) observed drift:          -23986
(D 1651074595.010166100) updateOffset
  (D 1651074595.013583400) one-way delay:           0s          0ns
  (D 1651074595.019435840) offset from master:      0s     -75315ns
  (D 1651074595.025288860) observed drift:          -29007
  (D 1651074596.010169680) updateOffset
(D 1651074596.013582440) one-way delay:           0s          0ns
  (D 1651074596.019431580) offset from master:      0s     -64788ns
  (D 1651074596.025279640) observed drift:          -33326
  (D 1651074597.010183160) updateOffset
  (D 1651074597.013598920) one-way delay:           0s          0ns
  (D 1651074597.019452760) offset from master:      0s     -52804ns
(D 1651074597.025303540) observed drift:          -36846
  (D 1651074598.010190380) handleSync: Q = 0, R = 4
  (D 1651074598.014670140) updateOffset
  (D 1651074598.018081040) one-way delay:           0s          0ns
  (D 1651074598.023931720) offset from master:      0s     -41322ns
(D 1651074598.029781100) observed drift:          -39600
  (D 1651074599.010207520) updateOffset
  (D 1651074599.013621040) one-way delay:           0s          0ns
  (D 1651074599.019473860) offset from master:      0s     -31571ns
  (D 1651074599.025327900) observed drift:          -41704
(D 1651074600.010208160) updateOffset
  (D 1651074600.013617080) one-way delay:           0s          0ns
  (D 1651074600.019464920) offset from master:      0s     -24075ns
  (D 1651074600.025313420) observed drift:          -43309
  (D 1651074601.010213640) updateOffset
(D 1651074601.013626040) one-way delay:           0s          0ns
  (D 1651074601.019476060) offset from master:      0s     -18798ns
  (D 1651074601.025324980) observed drift:          -44562
  (D 1651074602.010206680) handleSync: Q = 0, R = 4
  (D 1651074602.014686480) updateOffset
(D 1651074602.018090880) one-way delay:           0s          0ns
  (D 1651074602.023938740) offset from master:      0s     -15299ns
  (D 1651074602.029786700) observed drift:          -45581
  (D 1651074603.010212440) updateOffset
  (D 1651074603.013625860) one-way delay:           0s          0ns
(D 1651074603.019476660) offset from master:      0s     -13069ns
  (D 1651074603.025326720) observed drift:          -46452
  (D 1651074604.010215340) updateOffset
  (D 1651074604.013623100) one-way delay:           0s          0ns
  (D 1651074604.019472120) offset from master:      0s     -11605ns
(D 1651074604.025320480) observed drift:          -47225
  (D 1651074605.010213140) updateOffset
  (D 1651074605.013621840) one-way delay:           0s          0ns
  (D 1651074605.019470900) offset from master:      0s     -10492ns
  (D 1651074605.025319260) observed drift:          -47924
(D 1651074606.010199780) handleSync: Q = 0, R = 4
  (D 1651074606.014671420) updateOffset
  (D 1651074606.018076320) one-way delay:           0s          0ns
  (D 1651074606.023924000) offset from master:      0s      -9486ns
  (D 1651074606.029771740) observed drift:          -48556
(D 1651074606.034837020) event MASTER_CLOCK_SELECTED
  (D 1651074606.039536500) state PTP_SLAVE
  (D 1651074607.010218960) updateOffset
  (D 1651074607.013634780) one-way delay:           0s          0ns
  (D 1651074607.019487340) offset from master:      0s      -8473ns
(D 1651074607.025341940) observed drift:          -49120
  (D 1651074608.010223680) updateOffset
  (D 1651074608.013634620) one-way delay:           0s          0ns
  (D 1651074608.019487140) offset from master:      0s      -7437ns
  (D 1651074608.025340920) observed drift:          -49615
(D 1651074609.010230540) updateOffset
  (D 1651074609.013639240) one-way delay:           0s          0ns
  (D 1651074609.019488340) offset from master:      0s      -6398ns
  (D 1651074609.025336620) observed drift:          -50041
  (D 1651074610.010316980) handleSync: Q = 0, R = 4
(D 1651074610.014811860) updateOffset
  (D 1651074610.018223400) one-way delay:           0s          0ns
  (D 1651074610.024073780) offset from master:      0s      -5399ns
  (D 1651074610.029921940) observed drift:          -50400
  (D 1651074610.035041700) updateDelay
(D 1651074611.010234880) updateOffset
  (D 1651074611.013655740) one-way delay:           0s        560ns
  (D 1651074611.019509560) offset from master:      0s      -4760ns
  (D 1651074611.025357760) observed drift:          -50717
  (D 1651074612.010226080) updateOffset
(D 1651074612.013634580) one-way delay:           0s        560ns
  (D 1651074612.019482460) offset from master:      0s      -4040ns
  (D 1651074612.025330080) observed drift:          -50986
  (D 1651074613.010218900) updateOffset
  (D 1651074613.013633040) one-way delay:           0s        560ns
(D 1651074613.019480940) offset from master:      0s      -3310ns
  (D 1651074613.025328900) observed drift:          -51206
  (D 1651074614.010274600) handleSync: Q = 0, R = 4
  (D 1651074614.014781000) updateOffset
  (D 1651074614.018189140) one-way delay:           0s        560ns
(D 1651074614.024042200) offset from master:      0s      -2665ns
  (D 1651074614.029893600) observed drift:          -51383
  (D 1651074614.035011100) updateDelay
  (D 1651074615.010239400) updateOffset
  (D 1651074615.013655900) one-way delay:           0s        565ns
  (D 1651074615.019505320) offset from master:      0s      -2125ns
(D 1651074615.025353460) observed drift:          -51524
  (D 1651074616.010227540) updateOffset
  (D 1651074616.013642960) one-way delay:           0s        565ns
  (D 1651074616.019496780) offset from master:      0s      -1715ns
  (D 1651074616.025344860) observed drift:          -51638
(D 1651074617.010223120) updateOffset
  (D 1651074617.013635980) one-way delay:           0s        565ns
  (D 1651074617.019487520) offset from master:      0s      -1400ns
  (D 1651074617.025335620) observed drift:          -51731
  (D 1651074618.010260740) handleSync: Q = 0, R = 4
(D 1651074618.014748420) updateOffset
  (D 1651074618.018161560) one-way delay:           0s        565ns
  (D 1651074618.024013060) offset from master:      0s      -1172ns
  (D 1651074618.029863860) observed drift:          -51809
  (D 1651074618.034981640) updateDelay
(D 1651074619.010244340) updateOffset
  (D 1651074619.013667640) one-way delay:           0s        567ns
  (D 1651074619.019519740) offset from master:      0s      -1020ns
  (D 1651074619.025375300) observed drift:          -51877
  (D 1651074620.010238040) updateOffset
(D 1651074620.013652620) one-way delay:           0s        567ns
  (D 1651074620.019509500) offset from master:      0s       -893ns
  (D 1651074620.025368000) observed drift:          -51936
  (D 1651074621.010231220) updateOffset
  (D 1651074621.013646480) one-way delay:           0s        567ns
(D 1651074621.019496320) offset from master:      0s       -780ns
  (D 1651074621.025346060) observed drift:          -51988
  (D 1651074622.010260040) handleSync: Q = 0, R = 4
  (D 1651074622.014751920) updateOffset
  (D 1651074622.018162220) one-way delay:           0s        567ns
(D 1651074622.024016680) offset from master:      0s       -674ns
  (D 1651074622.029866640) observed drift:          -52032
  (D 1651074622.034981900) updateDelay
  (D 1651074623.010244060) updateOffset
  (D 1651074623.013664480) one-way delay:           0s        568ns
(D 1651074623.019517820) offset from master:      0s       -581ns
  (D 1651074623.025368760) observed drift:          -52070
  (D 1651074624.010227260) updateOffset
  (D 1651074624.013637640) one-way delay:           0s        568ns
  (D 1651074624.019494140) offset from master:      0s       -504ns
(D 1651074624.025353000) observed drift:          -52103
  (D 1651074625.010218440) updateOffset
  (D 1651074625.013631660) one-way delay:           0s        568ns
  (D 1651074625.019486940) offset from master:      0s       -436ns
  (D 1651074625.025336360) observed drift:          -52132
(D 1651074626.010263600) handleSync: Q = 0, R = 4
  (D 1651074626.014755600) updateOffset
  (D 1651074626.018161180) one-way delay:           0s        568ns
  (D 1651074626.024009960) offset from master:      0s       -382ns
  (D 1651074626.029857980) observed drift:          -52157
  (D 1651074626.034967380) updateDelay
  (D 1651074627.010237260) updateOffset
  (D 1651074627.013654920) one-way delay:           0s        566ns
  (D 1651074627.019512640) offset from master:      0s       -314ns
  (D 1651074627.025367660) observed drift:          -52177
  (D 1651074628.010232000) updateOffset
  (D 1651074628.013645700) one-way delay:           0s        566ns
  (D 1651074628.019503440) offset from master:      0s       -270ns
  (D 1651074628.025361880) observed drift:          -52195
  (D 1651074629.010231220) updateOffset
  (D 1651074629.013648080) one-way delay:           0s        566ns
  (D 1651074629.019506260) offset from master:      0s       -228ns
  (D 1651074629.025360640) observed drift:          -52210
  (D 1651074630.010265460) handleSync: Q = 0, R = 4
  (D 1651074630.014750980) updateOffset
  (D 1651074630.018160500) one-way delay:           0s        566ns
  (D 1651074630.024013820) offset from master:      0s       -177ns
  (D 1651074630.029863920) observed drift:          -52221
  (D 1651074630.034971540) updateDelay
  (D 1651074631.010242760) updateOffset
  (D 1651074631.013663780) one-way delay:           0s        567ns
  (D 1651074631.019520240) offset from master:      0s       -142ns
  (D 1651074631.025371320) observed drift:          -52230
  (D 1651074632.010235660) updateOffset
  (D 1651074632.013655600) one-way delay:           0s        567ns
  (D 1651074632.019508900) offset from master:      0s       -125ns
  (D 1651074632.025362500) observed drift:          -52238
  (D 1651074633.010233480) updateOffset
  (D 1651074633.013648620) one-way delay:           0s        567ns
  (D 1651074633.019506200) offset from master:      0s        -96ns
  (D 1651074633.025360240) observed drift:          -52244
  (D 1651074634.010262300) handleSync: Q = 0, R = 4
  (D 1651074634.014755220) updateOffset
  (D 1651074634.018160120) one-way delay:           0s        567ns
  (D 1651074634.024008020) offset from master:      0s        -81ns
  (D 1651074634.029855800) observed drift:          -52249
  (D 1651074634.034962780) updateDelay
  (D 1651074635.010224140) updateOffset
  (D 1651074635.013639720) one-way delay:           0s        568ns
  (D 1651074635.019488860) offset from master:      0s        -65ns
  (D 1651074635.025336700) observed drift:          -52253
  (D 1651074636.010221340) updateOffset
  (D 1651074636.013628000) one-way delay:           0s        568ns
  (D 1651074636.019477480) offset from master:      0s        -46ns
  (D 1651074636.025325660) observed drift:          -52256
  (D 1651074637.010223860) updateOffset
  (D 1651074637.013633880) one-way delay:           0s        568ns
  (D 1651074637.019481960) offset from master:      0s        -47ns
  (D 1651074637.025329740) observed drift:          -52259
  (D 1651074638.010256200) handleSync: Q = 0, R = 4
  (D 1651074638.014745220) updateOffset
  (D 1651074638.018148720) one-way delay:           0s        568ns
  (D 1651074638.023996560) offset from master:      0s        -48ns
  (D 1651074638.029844220) observed drift:          -52262
  (D 1651074638.034956880) updateDelay
  (D 1651074639.010234880) updateOffset
  (D 1651074639.013645140) one-way delay:           0s        568ns
  (D 1651074639.019500400) offset from master:      0s        -48ns
  (D 1651074639.025350060) observed drift:          -52265
  (D 1651074640.010222640) updateOffset
  (D 1651074640.013631780) one-way delay:           0s        568ns
  (D 1651074640.019487040) offset from master:      0s        -28ns
  (D 1651074640.025337360) observed drift:          -52266
  (D 1651074641.010222280) updateOffset
  (D 1651074641.013635780) one-way delay:           0s        568ns
  (D 1651074641.019488280) offset from master:      0s        -18ns
  (D 1651074641.025336320) observed drift:          -52267
  (D 1651074642.010260000) handleSync: Q = 0, R = 4
  (D 1651074642.014753760) updateOffset
  (D 1651074642.018160020) one-way delay:           0s        568ns
  (D 1651074642.024009240) offset from master:      0s        -23ns
  (D 1651074642.029858500) observed drift:          -52268
  (D 1651074642.034965020) updateDelay
  (D 1651074643.010233720) updateOffset
  (D 1651074643.013650740) one-way delay:           0s        567ns
  (D 1651074643.019499440) offset from master:      0s        -25ns
  (D 1651074643.025349320) observed drift:          -52269
  (D 1651074644.010235520) updateOffset
  (D 1651074644.013650340) one-way delay:           0s        567ns
  (D 1651074644.019499880) offset from master:      0s        -26ns
  (D 1651074644.025349880) observed drift:          -52270
  (D 1651074645.010227720) updateOffset
  (D 1651074645.013642980) one-way delay:           0s        567ns
  (D 1651074645.019494320) offset from master:      0s        -16ns
  (D 1651074645.025344360) observed drift:          -52271
  (D 1651074646.010256640) handleSync: Q = 0, R = 4
  (D 1651074646.014753640) updateOffset
  (D 1651074646.018162060) one-way delay:           0s        567ns
  (D 1651074646.024016040) offset from master:      0s        -22ns
  (D 1651074646.029863860) observed drift:          -52272
  (D 1651074646.034970500) updateDelay
  (D 1651074647.010231560) updateOffset
  (D 1651074647.013649300) one-way delay:           0s        566ns
  (D 1651074647.019507000) offset from master:      0s        -14ns
  (D 1651074647.025362720) observed drift:          -52272
  (D 1651074648.010229080) updateOffset
  (D 1651074648.013643140) one-way delay:           0s        566ns
  (D 1651074648.019494440) offset from master:      0s        -20ns
  (D 1651074648.025345040) observed drift:          -52273
  (D 1651074649.010225760) updateOffset
  (D 1651074649.013644780) one-way delay:           0s        566ns
  (D 1651074649.019500940) offset from master:      0s        -23ns
  (D 1651074649.025351400) observed drift:          -52274
  (D 1651074650.010269460) handleSync: Q = 0, R = 4
  (D 1651074650.014763440) updateOffset
  (D 1651074650.018172780) one-way delay:           0s        566ns
  (D 1651074650.024020540) offset from master:      0s        -14ns
  (D 1651074650.029868160) observed drift:          -52274
  (D 1651074650.034975940) updateDelay
  (D 1651074651.010225500) updateOffset
  (D 1651074651.013636400) one-way delay:           0s        568ns
  (D 1651074651.019484180) offset from master:      0s        -21ns
  (D 1651074651.025331880) observed drift:          -52275
  (D 1651074652.010221860) updateOffset
  (D 1651074652.013629880) one-way delay:           0s        568ns
  (D 1651074652.019478940) offset from master:      0s        -15ns
  (D 1651074652.025326720) observed drift:          -52276
  (D 1651074653.010222260) updateOffset
  (D 1651074653.013635200) one-way delay:           0s        568ns
  (D 1651074653.019483060) offset from master:      0s        -11ns
  (D 1651074653.025330840) observed drift:          -52276
  (D 1651074654.010264020) handleSync: Q = 0, R = 4
  (D 1651074654.014752220) updateOffset
  (D 1651074654.018166480) one-way delay:           0s        568ns
  (D 1651074654.024024660) offset from master:      0s        -20ns
  (D 1651074654.029882040) observed drift:          -52277
  (D 1651074654.034988440) updateDelay
  (D 1651074655.010235260) updateOffset
  (D 1651074655.013646880) one-way delay:           0s        568ns
  (D 1651074655.019496720) offset from master:      0s        -24ns
  (D 1651074655.025346900) observed drift:          -52278
  (D 1651074656.010219840) updateOffset
  (D 1651074656.013627100) one-way delay:           0s        568ns
  (D 1651074656.019480920) offset from master:      0s        -16ns
  (D 1651074656.025332620) observed drift:          -52279
  (D 1651074657.010220840) updateOffset
  (D 1651074657.013629140) one-way delay:           0s        568ns
  (D 1651074657.019477880) offset from master:      0s        -12ns
  (D 1651074657.025325680) observed drift:          -52279
  (D 1651074658.010261900) handleSync: Q = 0, R = 4
  (D 1651074658.014756360) updateOffset
  (D 1651074658.018163980) one-way delay:           0s        568ns
  (D 1651074658.024018280) offset from master:      0s        -10ns
  (D 1651074658.029869280) observed drift:          -52279
  (D 1651074658.034979180) updateDelay
  (D 1651074659.010225260) updateOffset
  (D 1651074659.013640360) one-way delay:           0s        569ns
  (D 1651074659.019491060) offset from master:      0s        -19ns
  (D 1651074659.025338600) observed drift:          -52280
  (D 1651074660.010225860) updateOffset
  (D 1651074660.013638080) one-way delay:           0s        569ns
  (D 1651074660.019490740) offset from master:      0s        -24ns
  (D 1651074660.025340140) observed drift:          -52281
  (D 1651074661.010220160) updateOffset
  (D 1651074661.013631220) one-way delay:           0s        569ns
  (D 1651074661.019481840) offset from master:      0s        -17ns
  (D 1651074661.025331020) observed drift:          -52282
  (D 1651074662.010269520) handleSync: Q = 0, R = 4
  (D 1651074662.014765400) updateOffset
  (D 1651074662.018171300) one-way delay:           0s        569ns
  (D 1651074662.024020320) offset from master:      0s         -3ns
  (D 1651074662.029868160) observed drift:          -52282
  (D 1651074662.034976860) updateDelay
  (D 1651074663.010228560) updateOffset
  (D 1651074663.013640460) one-way delay:           0s        571ns
  (D 1651074663.019488700) offset from master:      0s         -7ns
  (D 1651074663.025336200) observed drift:          -52282
  (D 1651074664.010226940) updateOffset
  (D 1651074664.013634660) one-way delay:           0s        571ns
  (D 1651074664.019488620) offset from master:      0s         -9ns
  (D 1651074664.025340800) observed drift:          -52282
  (D 1651074665.010226780) updateOffset
  (D 1651074665.013639920) one-way delay:           0s        571ns
  (D 1651074665.019491860) offset from master:      0s        -20ns
  (D 1651074665.025341080) observed drift:          -52283
  (D 1651074666.010259480) handleSync: Q = 0, R = 4
  (D 1651074666.014748740) updateOffset
  (D 1651074666.018153080) one-way delay:           0s        571ns
  (D 1651074666.024000920) offset from master:      0s        -25ns
  (D 1651074666.029848620) observed drift:          -52284
  (D 1651074666.034965520) updateDelay
  (D 1651074667.010242640) updateOffset
  (D 1651074667.013664340) one-way delay:           0s        571ns
  (D 1651074667.019516660) offset from master:      0s        -18ns
  (D 1651074667.025365320) observed drift:          -52285
  (D 1651074668.010228100) updateOffset
  (D 1651074668.013639840) one-way delay:           0s        571ns
  (D 1651074668.019494060) offset from master:      0s         -5ns
  (D 1651074668.025344040) observed drift:          -52285
  
  ```

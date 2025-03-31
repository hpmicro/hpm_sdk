.. _lwip_ptp_v1_slave:

lwip_ptp_v1_slave
==================================

概述
------

本示例展示PTP V1 Slave的功能

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

     This is an ethernet demo: PTP V1 Slave
     LwIP Version: 2.1.2
     Enet phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     IPv4 Address: 192.168.100.11
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.100.1
     (D 1651074123.993902300) event POWER UP
     (D 1651074123.997489380) initClock
     (D 1651074124.000649820) state PTP_INITIALIZING
     (D 1651074124.004933940) manufacturerIdentity: PTPd;1.0.1
     (D 1651074124.010095340) netShutdown
     (D 1651074124.013421580) netInit
     (D 1651074124.016526860) initData
     (D 1651074124.019606220) initTimer
     (D 1651074124.022750940) initClock
     (D 1651074124.025905620) sync message interval: 1000
     (D 1651074124.030634220) clock identifier: DFLT
     (D 1651074124.034926540) 256*log2(clock variance): -4000
     (D 1651074124.039995820) clock stratum: 255
     (D 1651074124.043927540) clock preferred?: no
     (D 1651074124.048035100) bound interface name:
     (D 1651074124.052314300) communication technology: 1
     (D 1651074124.057029420) uuid: dc:00:00:00:20:71
     (D 1651074124.061406540) PTP subdomain name: _DFLT
     (D 1651074124.065947540) subdomain address: 224.0.1.129
     (D 1651074124.070926140) event port address: 3f 1
     (D 1651074124.075381620) general port address: 40 1
     (D 1651074124.080016180) initClock
     (D 1651074124.083159380) state PTP_LISTENING
     (D 1651074124.087180620) initClock
     (D 1651074124.090327300) state PTP_SLAVE
     (D 1651074124.868703220) addForeign: new record (0,1) 1 1 cc:00:00:00:80:85
     (D 1651074124.875498660) event MASTER_CLOCK_CHANGED
     (D 1651074124.880117380) state PTP_UNCALIBRATED
     (D 1651074124.884387660) toState: Q = 0, R = 4
     (D 1651074125.868021740) updateOffset
     (D 1651074125.871449820) updateClock seconds
     (D 1651074131.029693340) setTime: resetting system clock to 1651074131s 29693040ns
     (D 1651074131.037027340) initClock
     (D 1651074131.040176420) one-way delay:           0s          0ns
     (D 1651074131.046027100) offset from master:      -5s -154233620ns
     (D 1651074131.051963740) observed drift:          0
     (D 1651074132.022183220) updateOffset
     (D 1651074132.025622140) one-way delay:           0s          0ns
     (D 1651074132.031472420) offset from master:      0s     -60440ns
     (D 1651074132.037320540) observed drift:          -4029
     (D 1651074133.022158920) updateOffset
     (D 1651074133.025579460) one-way delay:           0s          0ns
     (D 1651074133.031428620) offset from master:      0s     -72640ns
     (D 1651074133.037276680) observed drift:          -8871
     (D 1651074134.022127220) handleSync: Q = 0, R = 2
     (D 1651074134.026617500) updateOffset
     (D 1651074134.030030460) one-way delay:           0s          0ns
     (D 1651074134.035879680) offset from master:      0s     -85410ns
     (D 1651074134.041727880) observed drift:          -14565
     (D 1651074135.022131000) updateOffset
     (D 1651074135.025550520) one-way delay:           0s          0ns
     (D 1651074135.031399780) offset from master:      0s     -92475ns
     (D 1651074135.037248140) observed drift:          -20730
     (D 1651074136.022127940) handleSync: Q = 0, R = 2
     (D 1651074136.026608280) updateOffset
     (D 1651074136.030020940) one-way delay:           0s          0ns
     (D 1651074136.035869880) offset from master:      0s     -91798ns
     (D 1651074136.041718460) observed drift:          -26849
     (D 1651074137.022146280) updateOffset
     (D 1651074137.025568320) one-way delay:           0s          0ns
     (D 1651074137.031419120) offset from master:      0s     -84349ns
     (D 1651074137.037267600) observed drift:          -32472
     (D 1651074138.022159560) handleSync: Q = 0, R = 2
     (D 1651074138.026636640) updateOffset
     (D 1651074138.030048900) one-way delay:           0s          0ns
     (D 1651074138.035898340) offset from master:      0s     -72564ns
     (D 1651074138.041746680) observed drift:          -37309
     (D 1651074139.022181280) updateOffset
     (D 1651074139.025603400) one-way delay:           0s          0ns
     (D 1651074139.031456340) offset from master:      0s     -59132ns
     (D 1651074139.037307280) observed drift:          -41251
     (D 1651074140.022187100) handleSync: Q = 0, R = 2
     (D 1651074140.026660900) updateOffset
     (D 1651074140.030068620) one-way delay:           0s          0ns
     (D 1651074140.035916420) offset from master:      0s     -46256ns
     (D 1651074140.041764320) observed drift:          -44334
     (D 1651074141.022201800) updateOffset
     (D 1651074141.025622100) one-way delay:           0s          0ns
     (D 1651074141.031473800) offset from master:      0s     -35328ns
     (D 1651074141.037322460) observed drift:          -46689
     (D 1651074142.022198640) handleSync: Q = 0, R = 2
     (D 1651074142.026674560) updateOffset
     (D 1651074142.030085740) one-way delay:           0s          0ns
     (D 1651074142.035935060) offset from master:      0s     -26934ns
     (D 1651074142.041783300) observed drift:          -48484
     (D 1651074143.022206560) updateOffset
     (D 1651074143.025624700) one-way delay:           0s          0ns
     (D 1651074143.031477720) offset from master:      0s     -21007ns
     (D 1651074143.037333400) observed drift:          -49884
     (D 1651074144.022204680) handleSync: Q = 0, R = 2
     (D 1651074144.026683660) updateOffset
     (D 1651074144.030092240) one-way delay:           0s          0ns
     (D 1651074144.035940260) offset from master:      0s     -17094ns
     (D 1651074144.041788440) observed drift:          -51023
     (D 1651074145.022213260) updateOffset
     (D 1651074145.025630820) one-way delay:           0s          0ns
     (D 1651074145.031485320) offset from master:      0s     -14597ns
     (D 1651074145.037341040) observed drift:          -51996
     (D 1651074146.022207660) handleSync: Q = 0, R = 2
     (D 1651074146.026685580) updateOffset
     (D 1651074146.030100740) one-way delay:           0s          0ns
     (D 1651074146.035953800) offset from master:      0s     -12958ns
     (D 1651074146.041802760) observed drift:          -52859
     (D 1651074147.022217600) updateOffset
     (D 1651074147.025629640) one-way delay:           0s          0ns
     (D 1651074147.031482720) offset from master:      0s     -11709ns
     (D 1651074147.037338380) observed drift:          -53639
     (D 1651074148.022209300) handleSync: Q = 0, R = 2
     (D 1651074148.026685520) updateOffset
     (D 1651074148.030095640) one-way delay:           0s          0ns
     (D 1651074148.035945000) offset from master:      0s     -10585ns
     (D 1651074148.041795300) observed drift:          -54344
     (D 1651074149.022222380) updateOffset
     (D 1651074149.025633460) one-way delay:           0s          0ns
     (D 1651074149.031484560) offset from master:      0s      -9452ns
     (D 1651074149.037334600) observed drift:          -54974
     (D 1651074149.042400340) event MASTER_CLOCK_SELECTED
     (D 1651074149.047105780) state PTP_SLAVE
     (D 1651074150.022349200) handleSync: Q = 0, R = 2
     (D 1651074150.026861380) updateOffset
     (D 1651074150.030277440) one-way delay:           0s          0ns
     (D 1651074150.036131700) offset from master:      0s      -8286ns
     (D 1651074150.041987460) observed drift:          -55526
     (D 1651074150.047118240) updateDelay
     (D 1651074151.022244660) updateOffset
     (D 1651074151.025669380) one-way delay:           0s        560ns
     (D 1651074151.031528020) offset from master:      0s      -7383ns
     (D 1651074151.037383640) observed drift:          -56018
     (D 1651074152.022294860) handleSync: Q = 0, R = 2
     (D 1651074152.026798240) updateOffset
     (D 1651074152.030222160) one-way delay:           0s        560ns
     (D 1651074152.036076160) offset from master:      0s      -6312ns
     (D 1651074152.041924460) observed drift:          -56438
     (D 1651074152.047037160) updateDelay
     (D 1651074153.022235120) updateOffset
     (D 1651074153.025661720) one-way delay:           0s        565ns
     (D 1651074153.031526120) offset from master:      0s      -5218ns
     (D 1651074153.037376700) observed drift:          -56785
     (D 1651074154.022285480) handleSync: Q = 0, R = 2
     (D 1651074154.026782980) updateOffset
     (D 1651074154.030198080) one-way delay:           0s        565ns
     (D 1651074154.036049540) offset from master:      0s      -4212ns
     (D 1651074154.041900560) observed drift:          -57065
     (D 1651074154.047022820) updateDelay
     (D 1651074155.022246780) updateOffset
     (D 1651074155.025667980) one-way delay:           0s        567ns
     (D 1651074155.031523320) offset from master:      0s      -3369ns
     (D 1651074155.037380940) observed drift:          -57289
     (D 1651074156.022295180) handleSync: Q = 0, R = 2
     (D 1651074156.026792780) updateOffset
     (D 1651074156.030208820) one-way delay:           0s        567ns
     (D 1651074156.036062760) offset from master:      0s      -2698ns
     (D 1651074156.041913540) observed drift:          -57468
     (D 1651074156.047027400) updateDelay
     (D 1651074157.022244880) updateOffset
     (D 1651074157.025664760) one-way delay:           0s        568ns
     (D 1651074157.031518220) offset from master:      0s      -2183ns
     (D 1651074157.037369980) observed drift:          -57613
     (D 1651074158.022295760) handleSync: Q = 0, R = 2
     (D 1651074158.026793260) updateOffset
     (D 1651074158.030206820) one-way delay:           0s        568ns
     (D 1651074158.036059800) offset from master:      0s      -1816ns
     (D 1651074158.041908220) observed drift:          -57734
     (D 1651074158.047010360) updateDelay
     (D 1651074159.022246480) updateOffset
     (D 1651074159.025662440) one-way delay:           0s        563ns
     (D 1651074159.031516900) offset from master:      0s      -1539ns
     (D 1651074159.037367040) observed drift:          -57836
     (D 1651074160.022280360) handleSync: Q = 0, R = 2
     (D 1651074160.026776180) updateOffset
     (D 1651074160.030191540) one-way delay:           0s        563ns
     (D 1651074160.036043200) offset from master:      0s      -1331ns
     (D 1651074160.041891340) observed drift:          -57924
     (D 1651074160.046988680) updateDelay
     (D 1651074161.022237580) updateOffset
     (D 1651074161.025659820) one-way delay:           0s        563ns
     (D 1651074161.031508880) offset from master:      0s      -1167ns
     (D 1651074161.037357060) observed drift:          -58001
     (D 1651074162.022277680) handleSync: Q = 0, R = 2
     (D 1651074162.026779440) updateOffset
     (D 1651074162.030200880) one-way delay:           0s        563ns
     (D 1651074162.036057500) offset from master:      0s      -1025ns
     (D 1651074162.041905720) observed drift:          -58069
     (D 1651074162.047007260) updateDelay
     (D 1651074163.022242580) updateOffset
     (D 1651074163.025651380) one-way delay:           0s        564ns
     (D 1651074163.031503100) offset from master:      0s       -895ns
     (D 1651074163.037351040) observed drift:          -58128
     (D 1651074164.022282960) handleSync: Q = 0, R = 2
     (D 1651074164.026780900) updateOffset
     (D 1651074164.030198860) one-way delay:           0s        564ns
     (D 1651074164.036052260) offset from master:      0s       -769ns
     (D 1651074164.041900520) observed drift:          -58179
     (D 1651074164.047005020) updateDelay
     (D 1651074165.022235160) updateOffset
     (D 1651074165.025646800) one-way delay:           0s        565ns
     (D 1651074165.031497740) offset from master:      0s       -657ns
     (D 1651074165.037345900) observed drift:          -58222
     (D 1651074166.022270960) handleSync: Q = 0, R = 2
     (D 1651074166.026802220) updateOffset
     (D 1651074166.030221020) one-way delay:           0s        565ns
     (D 1651074166.036077800) offset from master:      0s       -551ns
     (D 1651074166.041926100) observed drift:          -58258
     (D 1651074166.047003240) updateDelay
     (D 1651074167.022234160) updateOffset
     (D 1651074167.025647120) one-way delay:           0s        566ns
     (D 1651074167.031497680) offset from master:      0s       -459ns
     (D 1651074167.037345980) observed drift:          -58288
     (D 1651074168.022290800) handleSync: Q = 0, R = 2
     (D 1651074168.026784560) updateOffset
     (D 1651074168.030199120) one-way delay:           0s        566ns
     (D 1651074168.036051740) offset from master:      0s       -372ns
     (D 1651074168.041901680) observed drift:          -58312
     (D 1651074168.046999940) updateDelay
     (D 1651074169.022239620) updateOffset
     (D 1651074169.025654900) one-way delay:           0s        566ns
     (D 1651074169.031505680) offset from master:      0s       -309ns
     (D 1651074169.037353700) observed drift:          -58332
     (D 1651074170.022282480) handleSync: Q = 0, R = 2
     (D 1651074170.026775380) updateOffset
     (D 1651074170.030190700) one-way delay:           0s        566ns
     (D 1651074170.036044240) offset from master:      0s       -258ns
     (D 1651074170.041892460) observed drift:          -58349
     (D 1651074170.046989600) updateDelay
     (D 1651074171.022240140) updateOffset
     (D 1651074171.025655340) one-way delay:           0s        567ns
     (D 1651074171.031508000) offset from master:      0s       -222ns
     (D 1651074171.037358320) observed drift:          -58363
     (D 1651074172.022279060) handleSync: Q = 0, R = 2
     (D 1651074172.026776640) updateOffset
     (D 1651074172.030201240) one-way delay:           0s        567ns
     (D 1651074172.036058380) offset from master:      0s       -185ns
     (D 1651074172.041909100) observed drift:          -58375
     (D 1651074172.047019720) updateDelay
     (D 1651074173.022239760) updateOffset
     (D 1651074173.025654960) one-way delay:           0s        566ns
     (D 1651074173.031505680) offset from master:      0s       -145ns
     (D 1651074173.037354100) observed drift:          -58384
     (D 1651074174.022284220) handleSync: Q = 0, R = 2
     (D 1651074174.026784160) updateOffset
     (D 1651074174.030205480) one-way delay:           0s        566ns
     (D 1651074174.036057360) offset from master:      0s       -126ns
     (D 1651074174.041905420) observed drift:          -58392
     (D 1651074174.047008800) updateDelay
     (D 1651074175.022231160) updateOffset
     (D 1651074175.025646360) one-way delay:           0s        565ns
     (D 1651074175.031497740) offset from master:      0s        -95ns
     (D 1651074175.037345840) observed drift:          -58398
     (D 1651074176.022266600) handleSync: Q = 0, R = 2
     (D 1651074176.026763580) updateOffset
     (D 1651074176.030178580) one-way delay:           0s        565ns
     (D 1651074176.036032100) offset from master:      0s        -80ns
     (D 1651074176.041880280) observed drift:          -58403
     (D 1651074176.046985820) updateDelay
     (D 1651074177.022241760) updateOffset
     (D 1651074177.025661280) one-way delay:           0s        566ns
     (D 1651074177.031512060) offset from master:      0s        -63ns
     (D 1651074177.037360100) observed drift:          -58407
     (D 1651074178.022274280) handleSync: Q = 0, R = 2
     (D 1651074178.026769540) updateOffset
     (D 1651074178.030184420) one-way delay:           0s        566ns
     (D 1651074178.036032460) offset from master:      0s        -45ns
     (D 1651074178.041880240) observed drift:          -58410
     (D 1651074178.046976280) updateDelay
     (D 1651074179.022229760) updateOffset
     (D 1651074179.025651120) one-way delay:           0s        565ns
     (D 1651074179.031512020) offset from master:      0s        -35ns
     (D 1651074179.037367040) observed drift:          -58412
     (D 1651074180.022265620) handleSync: Q = 0, R = 2
     (D 1651074180.026772640) updateOffset
     (D 1651074180.030192840) one-way delay:           0s        565ns
     (D 1651074180.036042560) offset from master:      0s        -20ns
     (D 1651074180.041890700) observed drift:          -58413
     (D 1651074180.046984760) updateDelay
     (D 1651074181.022233120) updateOffset
     (D 1651074181.025649000) one-way delay:           0s        565ns
     (D 1651074181.031503780) offset from master:      0s        -12ns
     (D 1651074181.037351620) observed drift:          -58413
     (D 1651074182.022272700) handleSync: Q = 0, R = 2
     (D 1651074182.026771060) updateOffset
     (D 1651074182.030187140) one-way delay:           0s        565ns
     (D 1651074182.036038120) offset from master:      0s        -19ns
     (D 1651074182.041886340) observed drift:          -58414
     (D 1651074182.046995840) updateDelay
     (D 1651074183.022237840) updateOffset
     (D 1651074183.025656880) one-way delay:           0s        564ns
     (D 1651074183.031513440) offset from master:      0s        -11ns
     (D 1651074183.037362820) observed drift:          -58414
     (D 1651074184.022271060) handleSync: Q = 0, R = 2
     (D 1651074184.026769580) updateOffset
     (D 1651074184.030191700) one-way delay:           0s        564ns
     (D 1651074184.036041040) offset from master:      0s         -8ns
     (D 1651074184.041891100) observed drift:          -58414
     (D 1651074184.046989920) updateDelay
     (D 1651074185.022235000) updateOffset
     (D 1651074185.025659560) one-way delay:           0s        564ns
     (D 1651074185.031507460) offset from master:      0s          4ns
     (D 1651074185.037355120) observed drift:          -58414
     (D 1651074186.022271100) handleSync: Q = 0, R = 2
     (D 1651074186.026764800) updateOffset
     (D 1651074186.030182280) one-way delay:           0s        564ns
     (D 1651074186.036034420) offset from master:      0s         10ns
     (D 1651074186.041882320) observed drift:          -58414
     (D 1651074186.046983140) updateDelay
     (D 1651074187.022232620) updateOffset
     (D 1651074187.025655580) one-way delay:           0s        564ns
     (D 1651074187.031505240) offset from master:      0s         13ns
     (D 1651074187.037353140) observed drift:          -58414
     (D 1651074188.022262160) handleSync: Q = 0, R = 2
     (D 1651074188.026775100) updateOffset
     (D 1651074188.030200860) one-way delay:           0s        564ns
     (D 1651074188.036052160) offset from master:      0s         15ns
     (D 1651074188.041900180) observed drift:          -58413
     (D 1651074188.046981040) updateDelay
     (D 1651074189.022237600) updateOffset
     (D 1651074189.025657280) one-way delay:           0s        565ns
     (D 1651074189.031507980) offset from master:      0s         15ns
     (D 1651074189.037356080) observed drift:          -58412
     (D 1651074190.022266640) handleSync: Q = 0, R = 2
     (D 1651074190.026783840) updateOffset
     (D 1651074190.030208440) one-way delay:           0s        565ns
     (D 1651074190.036058860) offset from master:      0s         15ns
     (D 1651074190.041906840) observed drift:          -58411
     (D 1651074190.046988100) updateDelay
     (D 1651074191.022243760) updateOffset
     (D 1651074191.025664240) one-way delay:           0s        564ns
     (D 1651074191.031513560) offset from master:      0s         25ns
     (D 1651074191.037361580) observed drift:          -58410
     (D 1651074192.022267000) handleSync: Q = 0, R = 2
     (D 1651074192.026788080) updateOffset
     (D 1651074192.030214880) one-way delay:           0s        564ns
     (D 1651074192.036067660) offset from master:      0s         31ns
     (D 1651074192.041916120) observed drift:          -58408
     (D 1651074192.046993100) updateDelay
     (D 1651074193.022242080) updateOffset
     (D 1651074193.025659440) one-way delay:           0s        564ns
     (D 1651074193.031511940) offset from master:      0s         33ns
     (D 1651074193.037359960) observed drift:          -58406
     (D 1651074194.022273560) handleSync: Q = 0, R = 2
     (D 1651074194.026770340) updateOffset
     (D 1651074194.030187500) one-way delay:           0s        564ns
     (D 1651074194.036040960) offset from master:      0s         35ns
     (D 1651074194.041889340) observed drift:          -58404
     (D 1651074194.046990840) updateDelay
     (D 1651074195.022238820) updateOffset
     (D 1651074195.025663060) one-way delay:           0s        564ns
     (D 1651074195.031512240) offset from master:      0s         35ns
     (D 1651074195.037360380) observed drift:          -58402
     (D 1651074196.022269800) handleSync: Q = 0, R = 2
     (D 1651074196.026787700) updateOffset
     (D 1651074196.030206820) one-way delay:           0s        564ns
     (D 1651074196.036061200) offset from master:      0s         46ns
     (D 1651074196.041909100) observed drift:          -58399
     (D 1651074196.046985440) updateDelay
     (D 1651074197.022238900) updateOffset
     (D 1651074197.025653700) one-way delay:           0s        564ns
     (D 1651074197.031504200) offset from master:      0s         41ns
     (D 1651074197.037352380) observed drift:          -58397
     (D 1651074198.022269840) handleSync: Q = 0, R = 2
     (D 1651074198.026767380) updateOffset
     (D 1651074198.030185860) one-way delay:           0s        564ns
     (D 1651074198.036039280) offset from master:      0s         28ns
     (D 1651074198.041887700) observed drift:          -58396
     (D 1651074198.046983120) updateDelay
     (D 1651074199.022242060) updateOffset
     (D 1651074199.025656060) one-way delay:           0s        563ns
     (D 1651074199.031505040) offset from master:      0s         33ns
     (D 1651074199.037353140) observed drift:          -58394
     (D 1651074200.022269600) handleSync: Q = 0, R = 2
     (D 1651074200.026792060) updateOffset
     (D 1651074200.030217340) one-way delay:           0s        563ns
     (D 1651074200.036074440) offset from master:      0s         35ns
     (D 1651074200.041931220) observed drift:          -58392
     (D 1651074200.047010480) updateDelay
     (D 1651074201.022237760) updateOffset
     (D 1651074201.025656240) one-way delay:           0s        563ns
     (D 1651074201.031511220) offset from master:      0s         36ns
     (D 1651074201.037367180) observed drift:          -58390
     (D 1651074202.022276420) handleSync: Q = 0, R = 2
     (D 1651074202.026770640) updateOffset
     (D 1651074202.030187720) one-way delay:           0s        563ns
     (D 1651074202.036036820) offset from master:      0s         36ns
     (D 1651074202.041884720) observed drift:          -58388
     (D 1651074202.046981540) updateDelay
     (D 1651074203.022239940) updateOffset
     (D 1651074203.025653180) one-way delay:           0s        563ns
     (D 1651074203.031504080) offset from master:      0s         27ns
     (D 1651074203.037353680) observed drift:          -58387
     (D 1651074204.022274080) handleSync: Q = 0, R = 2
     (D 1651074204.026769060) updateOffset
     (D 1651074204.030191980) one-way delay:           0s        563ns
     (D 1651074204.036042840) offset from master:      0s         32ns
     (D 1651074204.041893320) observed drift:          -58385
     (D 1651074204.046992440) updateDelay
     (D 1651074205.022238180) updateOffset
     (D 1651074205.025649980) one-way delay:           0s        564ns
     (D 1651074205.031499600) offset from master:      0s         34ns
     (D 1651074205.037348980) observed drift:          -58383
     (D 1651074206.022273420) handleSync: Q = 0, R = 2
     (D 1651074206.026776560) updateOffset
     (D 1651074206.030201800) one-way delay:           0s        564ns
     (D 1651074206.036054660) offset from master:      0s         35ns
     (D 1651074206.041905240) observed drift:          -58381
     (D 1651074206.047009460) updateDelay
     (D 1651074207.022241700) updateOffset
     (D 1651074207.025657980) one-way delay:           0s        563ns
     (D 1651074207.031517000) offset from master:      0s         26ns
     (D 1651074207.037371820) observed drift:          -58380
     (D 1651074208.022269020) handleSync: Q = 0, R = 2
     (D 1651074208.026778420) updateOffset
     (D 1651074208.030195660) one-way delay:           0s        563ns
     (D 1651074208.036053720) offset from master:      0s         31ns
     (D 1651074208.041908660) observed drift:          -58378
     (D 1651074208.047015120) updateDelay
     (D 1651074209.022237600) updateOffset
     (D 1651074209.025652580) one-way delay:           0s        563ns
     (D 1651074209.031505660) offset from master:      0s         44ns
     (D 1651074209.037355880) observed drift:          -58376
     (D 1651074210.022269700) handleSync: Q = 0, R = 2
     (D 1651074210.026770240) updateOffset
     (D 1651074210.030191760) one-way delay:           0s        563ns
     (D 1651074210.036045440) offset from master:      0s         41ns
     (D 1651074210.041896540) observed drift:          -58374
     (D 1651074210.046996140) updateDelay
     (D 1651074211.022243140) updateOffset
     (D 1651074211.025659540) one-way delay:           0s        564ns
     (D 1651074211.031507960) offset from master:      0s         38ns
     (D 1651074211.037355900) observed drift:          -58372
     (D 1651074212.022273620) handleSync: Q = 0, R = 2
     (D 1651074212.026770720) updateOffset
     (D 1651074212.030189840) one-way delay:           0s        564ns
     (D 1651074212.036048860) offset from master:      0s         27ns
     (D 1651074212.041896960) observed drift:          -58371
     (D 1651074212.046990100) updateDelay
     (D 1651074213.022234920) updateOffset
     (D 1651074213.025652640) one-way delay:           0s        563ns
     (D 1651074213.031503380) offset from master:      0s         32ns
     (D 1651074213.037351380) observed drift:          -58369
     (D 1651074214.022271240) handleSync: Q = 0, R = 2
     (D 1651074214.026775420) updateOffset
     (D 1651074214.030191500) one-way delay:           0s        563ns
     (D 1651074214.036048200) offset from master:      0s         25ns
     (D 1651074214.041896140) observed drift:          -58368
     (D 1651074214.046998200) updateDelay
     (D 1651074215.022241400) updateOffset
     (D 1651074215.025658400) one-way delay:           0s        563ns
     (D 1651074215.031511460) offset from master:      0s         31ns
     (D 1651074215.037359640) observed drift:          -58366
     (D 1651074216.022272320) handleSync: Q = 0, R = 2
     (D 1651074216.026775780) updateOffset
     (D 1651074216.030195700) one-way delay:           0s        563ns
     (D 1651074216.036057280) offset from master:      0s         24ns
     (D 1651074216.041906700) observed drift:          -58365
     (D 1651074216.047004240) updateDelay
     (D 1651074217.022244300) updateOffset
     (D 1651074217.025659540) one-way delay:           0s        563ns
     (D 1651074217.031509920) offset from master:      0s         20ns
     (D 1651074217.037357900) observed drift:          -58364
     (D 1651074218.022276340) handleSync: Q = 0, R = 2
     (D 1651074218.026772200) updateOffset
     (D 1651074218.030185840) one-way delay:           0s        563ns
     (D 1651074218.036036540) offset from master:      0s         19ns
     (D 1651074218.041884800) observed drift:          -58363
     (D 1651074218.046983500) updateDelay
     (D 1651074219.022232880) updateOffset
     (D 1651074219.025646720) one-way delay:           0s        563ns
     (D 1651074219.031496500) offset from master:      0s         18ns
     (D 1651074219.037344520) observed drift:          -58362
     (D 1651074220.022270600) handleSync: Q = 0, R = 2
     (D 1651074220.026764380) updateOffset
     (D 1651074220.030179940) one-way delay:           0s        563ns
     (D 1651074220.036035680) offset from master:      0s         27ns
     (D 1651074220.041884020) observed drift:          -58361
     (D 1651074220.046985640) updateDelay
     (D 1651074221.022241580) updateOffset
     (D 1651074221.025657740) one-way delay:           0s        563ns
     (D 1651074221.031509000) offset from master:      0s         32ns
     (D 1651074221.037356880) observed drift:          -58359
     (D 1651074222.022273620) handleSync: Q = 0, R = 2
     (D 1651074222.026767720) updateOffset
     (D 1651074222.030189560) one-way delay:           0s        563ns
     (D 1651074222.036040300) offset from master:      0s         35ns
     (D 1651074222.041888960) observed drift:          -58357
     (D 1651074222.046991100) updateDelay
     (D 1651074223.022240400) updateOffset
     (D 1651074223.025655320) one-way delay:           0s        563ns
     (D 1651074223.031504380) offset from master:      0s         46ns
     (D 1651074223.037352320) observed drift:          -58354
     (D 1651074224.022270680) handleSync: Q = 0, R = 2
     (D 1651074224.026760180) updateOffset
     (D 1651074224.030173420) one-way delay:           0s        563ns
     (D 1651074224.036029920) offset from master:      0s         51ns
     (D 1651074224.041878460) observed drift:          -58351
     (D 1651074224.046981400) updateDelay
     (D 1651074225.022231840) updateOffset
     (D 1651074225.025650000) one-way delay:           0s        563ns
     (D 1651074225.031504140) offset from master:      0s         34ns
     (D 1651074225.037352320) observed drift:          -58349
     (D 1651074226.022281340) handleSync: Q = 0, R = 2
     (D 1651074226.026770800) updateOffset
     (D 1651074226.030184960) one-way delay:           0s        563ns
     (D 1651074226.036039340) offset from master:      0s         26ns
     (D 1651074226.041887840) observed drift:          -58348
     (D 1651074226.046989140) updateDelay



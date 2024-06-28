# QEOV2_WAVE_OUT

## Overview

QEO_WAVE_OUT sample shows QEO generates waveforms according to received postion.


## Running the example

When the project runs correctly, the serial port terminal will receive the following information:
```console
QEO DAC wave example
QEO generate wave with sofeware inject postion
qeo wave0 output:
65535
65526
65496
65447
65378
65290
65182
65054
64907
64740
64554
64349
64126
63883
63621
63342
63043
62726
...
qeo wave1 output:
16384
15696
15015
14344
13684
13036
12400
11777
11165
10567
9982
9410
8854
8311
7783
7270
...
qeo wave2 output:
16382
17081
17792
18512
19240
19976
20721
21472
22231
22995
23766
24542
25323
26108
26897
27690
28486
...
```
sample printf wave value，using tool(like excel)to fit data and generate waveforms.
![](doc/qeo_dac_1.png)



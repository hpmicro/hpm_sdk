# List of RPC Commands implmenented in ESP-Hosted

A list of RPC Commands implemented in ESP-Hosted based on release version.

The RPC IDs listed here are for requests and events. Each request will have a corresponding response.

## List of RPC Requests

| No. | RPC ID | RPC Command                        | Added in Release |
|----:|-------:|------------------------------------|------------------|
|   1 |    257 | GetMacAddress                      | 0.0.6            |
|   2 |    258 | SetMacAddress                      | 0.0.6            |
|   3 |    259 | GetMode                            | 0.0.6            |
|   4 |    260 | SetMode                            | 0.0.6            |
|   5 |    261 | SuppDppInit                        | 2.4.3            |
|   6 |    262 | SuppDppDeinit                      | 2.4.3            |
|   7 |    263 | SuppDppBootstrapGen                | 2.4.3            |
|   8 |    264 | SuppDppStartListen                 | 2.4.3            |
|   9 |    265 | SuppDppStopListen                  | 2.4.3            |
|  10 |    270 | SetPs                              | 0.0.6            |
|  11 |    271 | GetPs                              | 0.0.6            |
|  12 |    272 | OTABegin                           | 0.0.6            |
|  13 |    273 | OTAWrite                           | 0.0.6            |
|  14 |    274 | OTAEnd                             | 0.0.6            |
|  15 |    275 | WifiSetMaxTxPower                  | 0.0.6            |
|  16 |    276 | WifiGetMaxTxPower                  | 0.0.6            |
|  17 |    277 | ConfigHeartbeat                    | 0.0.6            |
|  18 |    278 | WifiInit                           | 0.0.6            |
|  19 |    279 | WifiDeinit                         | 0.0.6            |
|  20 |    280 | WifiStart                          | 0.0.6            |
|  21 |    281 | WifiStop                           | 0.0.6            |
|  22 |    282 | WifiConnect                        | 0.0.6            |
|  23 |    283 | WifiDisconnect                     | 0.0.6            |
|  24 |    284 | WifiSetConfig                      | 0.0.6            |
|  25 |    285 | WifiGetConfig                      | 0.0.6            |
|  26 |    286 | WifiScanStart                      | 0.0.6            |
|  27 |    287 | WifiScanStop                       | 0.0.6            |
|  28 |    288 | WifiScanGetApNum                   | 0.0.6            |
|  29 |    289 | WifiScanGetApRecords               | 0.0.6            |
|  30 |    290 | WifiClearApList                    | 0.0.6            |
|  31 |    291 | WifiRestore                        | 0.0.6            |
|  32 |    292 | WifiClearFastConnect               | 0.0.6            |
|  33 |    293 | WifiDeauthSta                      | 0.0.6            |
|  34 |    294 | WifiStaGetApInfo                   | 0.0.6            |
|  35 |    297 | WifiSetProtocol                    | 0.0.6            |
|  36 |    298 | WifiGetProtocol                    | 0.0.6            |
|  37 |    299 | WifiSetBandwidth                   | 0.0.6            |
|  38 |    300 | WifiGetBandwidth                   | 0.0.6            |
|  39 |    301 | WifiSetChannel                     | 0.0.6            |
|  40 |    302 | WifiGetChannel                     | 0.0.6            |
|  41 |    303 | WifiSetCountry                     | 0.0.6            |
|  42 |    304 | WifiGetCountry                     | 0.0.6            |
|  43 |    311 | WifiApGetStaList                   | 0.0.6            |
|  44 |    312 | WifiApGetStaAid                    | 0.0.6            |
|  45 |    313 | WifiSetStorage                     | 0.0.6            |
|  46 |    325 | WifiSetInactiveTime                | 2.2.3            |
|  47 |    326 | WifiGetInactiveTime                | 2.2.3            |
|  48 |    334 | WifiSetCountryCode                 | 0.0.6            |
|  49 |    335 | WifiGetCountryCode                 | 0.0.6            |
|  50 |    338 | WifiStaGetAid                      | 0.0.10           |
|  51 |    339 | WifiStaGetNegotiatedPhymode        | 1.1.3            |
|  52 |    341 | WifiStaGetRssi                     | 0.0.6            |
|  53 |    342 | WifiSetProtocols                   | 0.0.10           |
|  54 |    343 | WifiGetProtocols                   | 0.0.10           |
|  55 |    344 | WifiSetBandwidths                  | 0.0.10           |
|  56 |    345 | WifiGetBandwidths                  | 0.0.10           |
|  57 |    346 | WifiSetBand                        | 0.0.10           |
|  58 |    347 | WifiGetBand                        | 0.0.10           |
|  59 |    348 | WifiSetBandMode                    | 0.0.10           |
|  60 |    349 | WifiGetBandMode                    | 0.0.10           |
|  61 |    350 | GetCoprocessorFwVersion            | 1.0.0            |
|  62 |    351 | WifiScanGetApRecord                | 1.1.3            |
|  63 |    352 | SetDhcpDnsStatus                   | 2.1.0            |
|  64 |    353 | GetDhcpDnsStatus                   | 2.1.0            |
|  65 |    354 | WifiStaTwtConfig                   | 2.1.2            |
|  66 |    355 | WifiStaItwtSetup                   | 2.1.2            |
|  67 |    356 | WifiStaItwtTeardown                | 2.1.2            |
|  68 |    357 | WifiStaItwtSuspend                 | 2.1.2            |
|  69 |    358 | WifiStaItwtGetFlowIdStatus         | 2.1.2            |
|  70 |    359 | WifiStaItwtSendProbeReq            | 2.1.2            |
|  71 |    360 | WifiStaItwtSetTargetWakeTimeOffset | 2.1.2            |
|  72 |    361 | WifiStaEnterpriseEnable            | 2.4.0            |
|  73 |    362 | WifiStaEnterpriseDisable           | 2.4.0            |
|  74 |    363 | EapSetIdentity                     | 2.4.0            |
|  75 |    364 | EapClearIdentity                   | 2.4.0            |
|  76 |    365 | EapSetUsername                     | 2.4.0            |
|  77 |    366 | EapClearUsername                   | 2.4.0            |
|  78 |    367 | EapSetPassword                     | 2.4.0            |
|  79 |    368 | EapClearPassword                   | 2.4.0            |
|  80 |    369 | EapSetNewPassword                  | 2.4.0            |
|  81 |    370 | EapClearNewPassword                | 2.4.0            |
|  82 |    371 | EapSetCaCert                       | 2.4.0            |
|  83 |    372 | EapClearCaCert                     | 2.4.0            |
|  84 |    373 | EapSetCertificateAndKey            | 2.4.0            |
|  85 |    374 | EapClearCertificateAndKey          | 2.4.0            |
|  86 |    375 | EapGetDisableTimeCheck             | 2.4.0            |
|  87 |    376 | EapSetTtlsPhase2Method             | 2.4.0            |
|  88 |    377 | EapSetSuiteb192bitCertification    | 2.4.0            |
|  89 |    378 | EapSetPacFile                      | 2.4.0            |
|  90 |    379 | EapSetFastParams                   | 2.4.0            |
|  91 |    380 | EapUseDefaultCertBundle            | 2.4.0            |
|  92 |    381 | WifiSetOkcSupport                  | 2.4.0            |
|  93 |    382 | EapSetDomainName                   | 2.4.0            |
|  94 |    383 | EapSetDisableTimeCheck             | 2.4.0            |
|  95 |    384 | EapSetEapMethods                   | 2.4.0            |
|  96 |    385 | IfaceMacAddrSetGet                 | 2.4.4            |
|  97 |    386 | IfaceMacAddrLenGet                 | 2.4.4            |
|  98 |    387 | FeatureControl                     | 2.4.4            |

## List of RPC Events

| No. | RPC ID | RPC Command        | Added in Release |
|----:|-------:|--------------------|------------------|
|   1 |    769 | ESPInit            | 0.0.6            |
|   2 |    770 | Heartbeat          | 0.0.6            |
|   3 |    771 | AP_StaConnected    | 0.0.6            |
|   4 |    772 | AP_StaDisconnected | 0.0.6            |
|   5 |    773 | WifiEventNoArgs    | 0.0.6            |
|   6 |    774 | StaScanDone        | 0.0.6            |
|   7 |    775 | StaConnected       | 0.0.6            |
|   8 |    776 | StaDisconnected    | 0.0.6            |
|   9 |    777 | DhcpDnsStatus      | 2.0.17           |
|  10 |    778 | StaItwtSetup       | 2.2.2            |
|  11 |    779 | StaItwtTeardown    | 2.2.2            |
|  12 |    780 | StaItwtSuspend     | 2.2.2            |
|  13 |    781 | StaItwtProbe       | 2.2.2            |
|  14 |    782 | SuppDppUriReady    | 2.4.3            |
|  15 |    783 | SuppDppCfgRecvd    | 2.4.3            |
|  16 |    784 | SuppDppFail        | 2.4.3            |
|  17 |    785 | WifiDppUriReady    | 2.4.3            |
|  18 |    786 | WifiDppCfgRecvd    | 2.4.3            |
|  19 |    787 | WifiDppFail        | 2.4.3            |

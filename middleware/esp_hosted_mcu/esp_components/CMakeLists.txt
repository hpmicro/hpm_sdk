# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause


sdk_inc(.)
sdk_inc(esp_common/include)
sdk_inc(esp_netif/include)
sdk_inc(esp_wifi/include)
sdk_inc(log)
sdk_inc(utils)
sdk_inc(esp_event/include)
sdk_src_glob(esp_wifi_remote/*.c)
sdk_src_glob(log/*.c)
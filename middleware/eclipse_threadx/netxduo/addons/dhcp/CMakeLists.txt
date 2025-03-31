# Copyright (c) 2023-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_definitions(-DCONFIG_NETX_ENABLE_DHCP=1)

sdk_src(nxd_dhcp_client.c)
sdk_src(nxd_dhcp_server.c)
sdk_src(nxd_dhcpv6_client.c)
sdk_src(nxd_dhcpv6_server.c)

sdk_inc(.)

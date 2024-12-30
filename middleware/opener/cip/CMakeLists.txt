# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_inc(../ports/hpmicro)
sdk_inc(../ports/hpmicro/sample_application)

sdk_src(
    appcontype.c
    cipassembly.c
    cipclass3connection.c
    cipcommon.c
    cipconnectionobject.c
    cipconnectionmanager.c
    cipdlr.c
    cipethernetlink.c
    cipidentity.c
    cipioconnection.c
    cipmessagerouter.c
    ciptcpipinterface.c
    cipepath.c
    cipelectronickey.c
    cipstring.c
    cipstringi.c
    cipqos.c
    ciptypes.c
)

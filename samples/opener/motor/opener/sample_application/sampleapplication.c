/*******************************************************************************
 * Copyright (c) 2012, Rockwell Automation, Inc.
 * All rights reserved.
 *
 ******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "opener_api.h"
#include "appcontype.h"
#include "trace.h"
#include "cipidentity.h"
#include "ciptcpipinterface.h"
#include "cipqos.h"
#include "cmsis_os2.h"
#include "hpm_gpio_drv.h"
#include "bldc_foc.h"
#define DEMO_APP_INPUT_ASSEMBLY_NUM                 100
#define DEMO_APP_OUTPUT_ASSEMBLY_NUM                150
#define DEMO_APP_CONFIG_ASSEMBLY_NUM                151
#define DEMO_APP_HEARTBEAT_INPUT_ONLY_ASSEMBLY_NUM  152
#define DEMO_APP_HEARTBEAT_LISTEN_ONLY_ASSEMBLY_NUM 153
#define DEMO_APP_EXPLICT_ASSEMBLY_NUM               154

/* global variables for demo application (4 assembly data fields)  ************/

EipUint8 g_assembly_data064[32]; /* Input */
EipUint8 g_assembly_data096[32]; /* Output */
EipUint8 g_assembly_data097[10]; /* Config */
EipUint8 g_assembly_data09A[32]; /* Explicit */

/* local functions */

/* global functions called by the stack */
EipStatus ApplicationInitialization(void)
{
    /* create 3 assembly object instances */
    /* INPUT */
    CreateAssemblyObject(DEMO_APP_INPUT_ASSEMBLY_NUM, g_assembly_data064, sizeof(g_assembly_data064));

    /* OUTPUT */
    CreateAssemblyObject(DEMO_APP_OUTPUT_ASSEMBLY_NUM, g_assembly_data096, sizeof(g_assembly_data096));

    /* CONFIG */
    CreateAssemblyObject(DEMO_APP_CONFIG_ASSEMBLY_NUM, g_assembly_data097, sizeof(g_assembly_data097));

    /* Heart-beat output assembly for Input only connections */
    CreateAssemblyObject(DEMO_APP_HEARTBEAT_INPUT_ONLY_ASSEMBLY_NUM, NULL, 0);

    /* Heart-beat output assembly for Listen only connections */
    CreateAssemblyObject(DEMO_APP_HEARTBEAT_LISTEN_ONLY_ASSEMBLY_NUM, NULL, 0);

    /* assembly for explicit messaging */
    CreateAssemblyObject(DEMO_APP_EXPLICT_ASSEMBLY_NUM, g_assembly_data09A, sizeof(g_assembly_data09A));

    ConfigureExclusiveOwnerConnectionPoint(0, DEMO_APP_OUTPUT_ASSEMBLY_NUM, DEMO_APP_INPUT_ASSEMBLY_NUM, DEMO_APP_CONFIG_ASSEMBLY_NUM);
    ConfigureInputOnlyConnectionPoint(0, DEMO_APP_HEARTBEAT_INPUT_ONLY_ASSEMBLY_NUM, DEMO_APP_INPUT_ASSEMBLY_NUM, DEMO_APP_CONFIG_ASSEMBLY_NUM);
    ConfigureListenOnlyConnectionPoint(0, DEMO_APP_HEARTBEAT_LISTEN_ONLY_ASSEMBLY_NUM, DEMO_APP_INPUT_ASSEMBLY_NUM, DEMO_APP_CONFIG_ASSEMBLY_NUM);

#if defined(OPENER_ETHLINK_CNTRS_ENABLE) && 0 != OPENER_ETHLINK_CNTRS_ENABLE
    /* For the Ethernet Interface & Media Counters connect a PreGetCallback and
     * a PostGetCallback.
     * The PreGetCallback is used to fetch the counters from the hardware.
     * The PostGetCallback is utilized by the GetAndClear service to clear
     * the hardware counters after the current data have been transmitted.
     */
    {
        CipClass *p_eth_link_class = GetCipClass(kCipEthernetLinkClassCode);
        InsertGetSetCallback(p_eth_link_class, EthLnkPreGetCallback, kPreGetFunc);
        InsertGetSetCallback(p_eth_link_class, EthLnkPostGetCallback, kPostGetFunc);
        /* Specify the attributes for which the callback should be executed. */
        for (int idx = 0; idx < OPENER_ETHLINK_INSTANCE_CNT; ++idx) {
            CipAttributeStruct *p_eth_link_attr;
            CipInstance *p_eth_link_inst = GetCipInstance(p_eth_link_class, idx + 1);
            OPENER_ASSERT(p_eth_link_inst);

            /* Interface counters attribute */
            p_eth_link_attr = GetCipAttribute(p_eth_link_inst, 4);
            p_eth_link_attr->attribute_flags |= (kPreGetFunc | kPostGetFunc);
            /* Media counters attribute */
            p_eth_link_attr = GetCipAttribute(p_eth_link_inst, 5);
            p_eth_link_attr->attribute_flags |= (kPreGetFunc | kPostGetFunc);
        }
    }
#endif

    return kEipStatusOk;
}

void HandleApplication(void)
{
    /* check if application needs to trigger an connection */
}

void CheckIoConnectionEvent(unsigned int output_assembly_id, unsigned int input_assembly_id, IoConnectionEvent io_connection_event)
{
    /* maintain a correct output state according to the connection state*/

    (void)output_assembly_id;  /* suppress compiler warning */
    (void)input_assembly_id;   /* suppress compiler warning */
    (void)io_connection_event; /* suppress compiler warning */
}
volatile uint8_t speed;
EipStatus AfterAssemblyDataReceived(CipInstance *instance)
{
    EipStatus status = kEipStatusOk;
    static bool help_info_printed = false;
    /* handle the data received e.g., update outputs of the device */
    if (!help_info_printed) {
        help_info_printed = true;
        printf("Mesaage receieved from host!\n");
    }
    switch (instance->instance_number) {
    case DEMO_APP_OUTPUT_ASSEMBLY_NUM:
        if (g_assembly_data096[4] != 0) {
            gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, BOARD_LED_ON_LEVEL);
        } else {
            gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, BOARD_LED_OFF_LEVEL);
        }
        if (g_assembly_data096[5] == 0)
            set_motor_speed(*((float *)g_assembly_data096));
        else
            set_motor_speed(0);

        break;
    case DEMO_APP_EXPLICT_ASSEMBLY_NUM:
        /* do something interesting with the new data from
         * the explicit set-data-attribute message */
        break;
    case DEMO_APP_CONFIG_ASSEMBLY_NUM:
        /* Add here code to handle configuration data and check if it is ok
         * The demo application does not handle config data.
         * However in order to pass the test we accept any data given.
         * EIP_ERROR
         */
        status = kEipStatusOk;
        break;
    default:
        OPENER_TRACE_INFO("Unknown assembly instance ind AfterAssemblyDataReceived");
        break;
    }
    return status;
}

EipBool8 BeforeAssemblyDataSend(CipInstance *pa_pstInstance)
{
    /* update data to be sent e.g., read inputs of the device */
    /* In this sample app we mirror the data from out to inputs on data receive
     * therefore we need nothing to do here. Just return true to inform that
     * the data is new.
     */

    if (pa_pstInstance->instance_number == DEMO_APP_EXPLICT_ASSEMBLY_NUM) {
        /* do something interesting with the existing data
         * for the explicit get-data-attribute message */
    }
    if (pa_pstInstance->instance_number == DEMO_APP_INPUT_ASSEMBLY_NUM) {
        *((float *)&g_assembly_data064[1]) = get_motor_current_speed();
        if (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN)) {
            g_assembly_data064[0] = 1;
        } else {
            g_assembly_data064[0] = 0;
        }
    }
    return true;
}

EipStatus ResetDevice(void)
{
    /* add reset code here */
    CloseAllConnections();
    CipQosUpdateUsedSetQosValues();
    return kEipStatusOk;
}

EipStatus ResetDeviceToInitialConfiguration(void)
{
    /* rest the parameters */
    g_tcpip.encapsulation_inactivity_timeout = 120;
    CipQosResetAttributesToDefaultValues();
    /* than perform device reset */
    ResetDevice();
    return kEipStatusOk;
}

void *CipCalloc(size_t number_of_elements, size_t size_of_element)
{
    void *temp = pvPortMalloc(number_of_elements * size_of_element);
    if (NULL != temp) {
        memset(temp, 0, number_of_elements * size_of_element);
    }
    return temp;
}

void CipFree(void *data)
{
    vPortFree(data);
}

void RunIdleChanged(EipUint32 run_idle_value)
{
    OPENER_TRACE_INFO("Run/Idle handler triggered\n");
    if ((0x0001 & run_idle_value) == 1) {
        CipIdentitySetExtendedDeviceStatus(kAtLeastOneIoConnectionInRunMode);
    } else {
        CipIdentitySetExtendedDeviceStatus(kAtLeastOneIoConnectionEstablishedAllInIdleMode);
    }
    (void)run_idle_value;
}

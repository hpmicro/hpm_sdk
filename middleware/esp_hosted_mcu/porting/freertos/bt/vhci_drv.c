// Copyright 2015-2024 Espressif Systems (Shanghai) PTE LTD
/* SPDX-License-Identifier: GPL-2.0 OR Apache-2.0 */

#include <stdint.h>

#include "port_esp_hosted_host_os.h"
#include "transport_drv.h"

#include "hci_drv.h"

#if H_BT_HOST_ESP_NIMBLE
#include <sysinit/sysinit.h>
#include <syscfg/syscfg.h>
#include "host/ble_hs_mbuf.h"
#include "os/os_mbuf.h"
#include "nimble/transport.h"
#include "nimble/transport/hci_h4.h"
#endif

#include "esp_hosted_log.h"

#define TAG "vhci_drv"

#if H_BT_HOST_ESP_NIMBLE
struct hci_h4_sm hci_h4sm;

static int hci_uart_frame_cb(uint8_t pkt_type, void *data)
{
    switch (pkt_type) {
    case HCI_H4_EVT:
        return ble_transport_to_hs_evt(data);
    case HCI_H4_ACL:
        return ble_transport_to_hs_acl(data);
    default:
        assert(0);
        break;
    }
    return -1;
}
#endif /* H_BT_HOST_ESP_NIMBLE */

/**
 * HCI_H4_xxx is the first byte of the received data
 */
int hci_rx_handler(interface_buffer_handle_t *buf_handle)
{
#if H_BT_HOST_ESP_NIMBLE
#if LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG
	/* format: TAG rx(<type>) */
	char head[sizeof(TAG) + sizeof("rx") + sizeof("none") + sizeof("()")];
	char *type;
	switch (buf_handle->payload[0])
	{
		case HCI_H4_CMD: type = "cmd"; break;
		case HCI_H4_ACL: type = "acl"; break;
		case HCI_H4_EVT: type = "evt"; break;
		case HCI_H4_ISO: type = "iso"; break;
		default: type = "none"; break;
	}
	snprintf(head, sizeof(head), "%s rx(%s)", TAG, type);
	ESP_LOG_BUFFER_HEXDUMP(head, buf_handle->payload, buf_handle->payload_len, ESP_LOG_DEBUG);
#endif /* DBG_LVL == DBG_LOG */
	hci_h4_sm_rx(&hci_h4sm, buf_handle->payload, buf_handle->payload_len);
#endif /* H_BT_HOST_ESP_NIMBLE */
	return ESP_OK;
}

void hci_drv_init(void)
{
#if H_BT_HOST_ESP_NIMBLE
	hci_h4_sm_init(&hci_h4sm, &hci_h4_allocs_from_ll, hci_uart_frame_cb);
#endif /* H_BT_HOST_ESP_NIMBLE */
}

ATTR_WEAK void esp_hosted_bt_startup (void)
{
    /* The callback indicates that the vhci interface is ready.
     * The user rewrites this function to initialize or configure the Bluetooth protocol stack.
     */
}

void hci_drv_show_configuration(void)
{
#if H_BT_HOST_ESP_NIMBLE
	ESP_LOGI(TAG, "Host BT Support: Enabled");
	ESP_LOGI(TAG, "\tBT Transport Type: VHCI");
	ESP_LOGI(TAG, "\tBT Stack Type: NimBLE");
#endif /* H_BT_HOST_ESP_NIMBLE */
	esp_hosted_bt_startup();
}

#if H_BT_HOST_ESP_NIMBLE
/**
 * ESP NimBLE expects these interfaces for Tx
 *
 * For doing non-zero copy:
 * - transport expects the HCI_H4_xxx type to be the first byte of the
 *   data stream
 *
 * For doing zero copy:
 * - fill in esp_paylod_header and payload data
 * - HCI_H4_xxx type should be set in esp_payload_header.hci_pkt_type
 */

#if H_BT_ENABLE_LL_INIT
void ble_transport_ll_init(void)
{
	ESP_ERROR_CHECK(transport_drv_reconfigure());
}
#endif

int ble_transport_to_ll_acl_impl(struct os_mbuf *om)
{
	int res;

		uint8_t *data = NULL;
		int data_len = OS_MBUF_PKTLEN(om) + 1;

		data = g_h.funcs->_h_malloc(data_len);
		if (!data) {
			ESP_LOGE(TAG, "tx(acl) %s: malloc failed", __func__);
			res = ESP_FAIL;
			goto exit;
		}

		data[0] = HCI_H4_ACL;
		res = ble_hs_mbuf_to_flat(om, &data[1], OS_MBUF_PKTLEN(om), NULL);
		if (res) {
			ESP_LOGE(TAG, "Tx: Error copying HCI_H4_ACL data %d", res);
			res = ESP_FAIL;
			goto exit;
		}

		ESP_LOG_BUFFER_HEXDUMP(TAG " tx(acl)", data, data_len, ESP_LOG_DEBUG);

		res = esp_hosted_tx(ESP_HCI_IF, 0, data, data_len, H_BUFF_NO_ZEROCOPY, H_DEFLT_FREE_FUNC);

 exit:
	os_mbuf_free_chain(om);

	return res;
}

int ble_transport_to_ll_cmd_impl(void *buf)
{
	// TODO: zerocopy version

	// calculate data length from the incoming data
	int buf_len = 3 + ((uint8_t *)buf)[2] + 1;

	uint8_t * data = NULL;
	int res;

	data = g_h.funcs->_h_malloc(buf_len);
	if (!data) {
		ESP_LOGE(TAG, "tx(cmd) %s: malloc failed", __func__);
		res =  ESP_FAIL;
		goto exit;
	}

	data[0] = HCI_H4_CMD;
	memcpy(&data[1], buf, buf_len - 1);

	ESP_LOG_BUFFER_HEXDUMP(TAG " tx(cmd)", data, buf_len, ESP_LOG_DEBUG);

	res = esp_hosted_tx(ESP_HCI_IF, 0, data, buf_len, H_BUFF_NO_ZEROCOPY, H_DEFLT_FREE_FUNC);

 exit:
	ble_transport_free(buf);

	return res;
}
#endif // H_BT_HOST_ESP_NIMBLE

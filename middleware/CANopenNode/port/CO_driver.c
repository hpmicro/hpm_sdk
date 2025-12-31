/*
 * Copyright (c) 2019 Vestas Wind Systems A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hpm_canopen.h"
#ifdef HPMSOC_HAS_HPMSDK_MCAN
#include "hpm_canopen_mcan.h"
#else
#include "hpm_canopen_can.h"
#endif
#include "can.h"
#include "CO_driver.h"

#define SAMPLE_POINT_MARGIN 50
#define CAN_SYNC_SEG 1

extern CO_t *co;

static void canopen_detach_all_rx_filters(CO_CANmodule_t *CANmodule)
{
	uint16_t i;

	if (!CANmodule || !CANmodule->rxArray || !CANmodule->configured) {
		return;
	}

	for (i = 0U; i < CANmodule->rxSize; i++) {
		if (CANmodule->rxArray[i].filter_id != -ENOSPC) {
			can_remove_rx_filter(CANmodule->CANptr,
					     CANmodule->rxArray[i].filter_id);
			CANmodule->rxArray[i].filter_id = -ENOSPC;
		}
	}
}

static void canopen_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data)
{
	CO_CANmodule_t *CANmodule = (CO_CANmodule_t *)user_data;
	CO_CANrxMsg_t rxMsg;
	CO_CANrx_t *buffer;
	int i;

	ARG_UNUSED(dev);

	/* Loop through registered rx buffers in priority order */
	for (i = 0; i < CANmodule->rxSize; i++) {
		buffer = &CANmodule->rxArray[i];

		if (buffer->ident == (uint16_t)-ENOSPC || buffer->CANrx_callback == NULL) {
			continue;
		}

		if (((frame->id ^ buffer->ident) & buffer->mask) == 0U) {
#ifdef CONFIG_CAN_ACCEPT_RTR
			if (buffer->rtr && ((frame->flags & CAN_FRAME_RTR) == 0U)) {
				continue;
			}
#endif /* CONFIG_CAN_ACCEPT_RTR */
			rxMsg.ident = frame->id;
			rxMsg.DLC = frame->dlc;
			memcpy(rxMsg.data, frame->data, frame->dlc);
			buffer->CANrx_callback(buffer->object, &rxMsg);
			break;
		}
	}
}

void CO_CANsetConfigurationMode(void *CANdriverState)
{
	struct canopen_context *ctx = (struct canopen_context *)CANdriverState;
	int err;

	err = can_stop((const struct device *)ctx->dev);
	if (err != 0 && err != -EALREADY) {
		printf("failed to stop CAN interface (err %d)\n", err);
	}
}

void CO_CANsetNormalMode(CO_CANmodule_t *CANmodule)
{
	int err;

	err = can_start(CANmodule->CANptr);
	if (err != 0 && err != -EALREADY) {
		printf("failed to start CAN interface (err %d)\n", err);
		return;
	}

	CANmodule->CANnormal = true;
}

CO_ReturnError_t CO_CANmodule_init(CO_CANmodule_t *CANmodule,
				   void *CANdriverState,
				   CO_CANrx_t rxArray[], uint16_t rxSize,
				   CO_CANtx_t txArray[], uint16_t txSize,
				   uint16_t CANbitRate)
{
	struct canopen_context *ctx = (struct canopen_context *)CANdriverState;
	uint16_t i;
	int max_filters;

	ARG_UNUSED(CANbitRate);

	printf("rxSize = %d, txSize = %d\n", rxSize, txSize);

	if (!CANmodule || !rxArray || !txArray || !CANdriverState) {
		printf("failed to initialize CAN module\n");
		return CO_ERROR_ILLEGAL_ARGUMENT;
	}

	max_filters = can_get_max_filters((const struct device *)ctx->dev, false);
	if (max_filters < 0) {
			printf("unable to determine number of CAN RX filters\n");
			return CO_ERROR_SYSCALL;
	}

	canopen_detach_all_rx_filters(CANmodule);

	CANmodule->CANptr = ctx->dev;
	CANmodule->rxArray = rxArray;
	CANmodule->rxSize = rxSize;
	CANmodule->txArray = txArray;
	CANmodule->txSize = txSize;
	CANmodule->CANnormal = false;
	CANmodule->firstCANtxMessage = true;
	CANmodule->errinfo = 0;

	for (i = 0U; i < rxSize; i++) {
		rxArray[i].ident = 0U;
		rxArray[i].CANrx_callback = NULL;
		rxArray[i].filter_id = -ENOSPC;
	}

	for (i = 0U; i < txSize; i++) {
		txArray[i].bufferFull = false;
	}

	CANmodule->configured = true;

	return CO_ERROR_NO;
}

void CO_CANmodule_disable(CO_CANmodule_t *CANmodule)
{
	int err;

	if (!CANmodule || !CANmodule->CANptr) {
		return;
	}

	canopen_detach_all_rx_filters(CANmodule);

	err = can_stop(CANmodule->CANptr);
	if (err != 0 && err != -EALREADY) {
		printf("failed to disable CAN interface (err %d)\n", err);
	}
}

int can_add_rx_filter(const struct device *dev, can_rx_callback_t callback,
		      void *user_data, const struct can_filter *filter)
{
	const struct can_driver_api *api = (const struct can_driver_api *)dev->api;
	uint32_t id_mask;

	if (callback == NULL || filter == NULL) {
		return -EINVAL;
	}

	if ((filter->flags & CAN_FILTER_IDE) != 0U) {
		id_mask = CAN_EXT_ID_MASK;
	} else {
		id_mask = CAN_STD_ID_MASK;
	}

	if (((filter->id & ~(id_mask)) != 0U) || ((filter->mask & ~(id_mask)) != 0U)) {
		return -EINVAL;
	}

	return api->add_rx_filter(dev, callback, user_data, filter);
}

uint16_t CO_CANrxMsg_readIdent(CO_CANrxMsg_t *rxMsg)
{
	return rxMsg->ident;
}

uint8_t CO_CANrxMsg_readDLC(CO_CANrxMsg_t *rxMsg)
{
    return rxMsg->DLC;
}

uint8_t *CO_CANrxMsg_readData(CO_CANrxMsg_t *rxMsg)
{
    return rxMsg->data;
}

CO_ReturnError_t CO_CANrxBufferInit(CO_CANmodule_t *CANmodule, uint16_t index,
				uint16_t ident, uint16_t mask, bool_t rtr,
				void *object,
				void (*CANrx_callback)(void *object, void *message))
{
	struct can_filter filter;
	CO_CANrx_t *buffer;

	if (CANmodule == NULL || !CANrx_callback || (index >= CANmodule->rxSize)) {
		return CO_ERROR_ILLEGAL_ARGUMENT;
	}

	 buffer = &CANmodule->rxArray[index];
	 buffer->object = object;
	 buffer->CANrx_callback = CANrx_callback;
	 buffer->ident = ident;
	 buffer->mask = mask;

#ifndef CONFIG_CAN_ACCEPT_RTR
	if (rtr) {
		printf("request for RTR frames, but RTR frames are rejected\n");
		CO_errorReport(co->em, CO_EM_GENERIC_SOFTWARE_ERROR,
			       CO_EMC_SOFTWARE_INTERNAL, 0);
		return CO_ERROR_ILLEGAL_ARGUMENT;
	}
#else /* !CONFIG_CAN_ACCEPT_RTR */
	buffer->rtr = rtr;
#endif /* CONFIG_CAN_ACCEPT_RTR */

	filter.flags = 0; /* Standard Identifier */
	filter.id = ident;
	filter.mask = mask;

	 if (buffer->filter_id != -ENOSPC) {
	 	can_remove_rx_filter(CANmodule->CANptr, buffer->filter_id);
	 }

	buffer->filter_id = can_add_rx_filter(CANmodule->CANptr,
					      canopen_rx_callback,
					      CANmodule, &filter);
	if (buffer->filter_id == -ENOSPC) {
		printf("failed to add CAN rx callback, no free filter\n");
		return CO_ERROR_OUT_OF_MEMORY;
	}

	return CO_ERROR_NO;
}

CO_CANtx_t *CO_CANtxBufferInit(CO_CANmodule_t *CANmodule, uint16_t index,
			       uint16_t ident, bool_t rtr, uint8_t noOfBytes,
			       bool_t syncFlag)
{
	CO_CANtx_t *buffer;

	if (CANmodule == NULL || index >= CANmodule->txSize) {
		return NULL;
	}

	buffer = &CANmodule->txArray[index];
	buffer->ident = ident;
	buffer->rtr = rtr;
	buffer->DLC = noOfBytes;
	buffer->bufferFull = false;
	buffer->syncFlag = syncFlag;

	return buffer;
}

int can_send(const struct device *dev, const struct can_frame *frame,
		    k_timeout_t timeout, can_tx_callback_t callback,
		    void *user_data)
{
	const struct can_driver_api *api = (const struct can_driver_api *)dev->api;
	uint32_t id_mask;

	if (frame == NULL) {
		return -EINVAL;
	}

	if ((frame->flags & CAN_FRAME_IDE) != 0U) {
		id_mask = CAN_EXT_ID_MASK;
	} else {
		id_mask = CAN_STD_ID_MASK;
	}

	if ((frame->id & ~(id_mask)) != 0U) {
		return -EINVAL;
	}

	return api->send(dev, frame, timeout, callback, user_data);
}

CO_ReturnError_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer)
{
	CO_ReturnError_t ret = CO_ERROR_NO;
	struct can_frame frame;
	int err;

	if (!CANmodule || !CANmodule->CANptr || !buffer) {
		return CO_ERROR_ILLEGAL_ARGUMENT;
	}

	memset(&frame, 0, sizeof(frame));

	if (buffer->bufferFull) {
		buffer->bufferFull = false;
		ret = CO_ERROR_TX_OVERFLOW;
	}

	frame.id = buffer->ident;
	frame.dlc = buffer->DLC;
	frame.flags = (buffer->rtr ? CAN_FRAME_RTR : 0);
	memcpy(frame.data, buffer->data, buffer->DLC);

	err = can_send(CANmodule->CANptr, &frame, K_NO_WAIT, NULL, CANmodule);
	if (err == -EAGAIN) {
		buffer->bufferFull = true;
	} else if (err != 0) {
		ret = CO_ERROR_TX_UNCONFIGURED;
	}

	return ret;
}

void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule)
{
	bool_t tpdoDeleted = false;
	CO_CANtx_t *buffer;
	uint16_t i;

	if (!CANmodule) {
		return;
	}

	for (i = 0; i < CANmodule->txSize; i++) {
		buffer = &CANmodule->txArray[i];
		if (buffer->bufferFull && buffer->syncFlag) {
			buffer->bufferFull = false;
			tpdoDeleted = true;
		}
	}

	if (tpdoDeleted) {
		CO_errorReport(co->em, CO_EM_TPDO_OUTSIDE_WINDOW,
			       CO_EMC_COMMUNICATION, 0);
	}
}


/******************************************************************************/
/* Get error counters from the module. If necessary, function may use different way to determine errors. */
static uint16_t rxErrors = 0, txErrors = 0, overflow = 0;

void CO_CANmodule_process(CO_CANmodule_t *CANmodule)
{
    uint32_t err;

    err = ((uint32_t)txErrors << 16) | ((uint32_t)rxErrors << 8) | overflow;

    if (CANmodule->errOld != err) {
        uint16_t status = CANmodule->CANerrorStatus;

        CANmodule->errOld = err;

        if (txErrors >= 256U) {
            /* bus off */
            status |= CO_CAN_ERRTX_BUS_OFF;
        } else {
            /* recalculate CANerrorStatus, first clear some flags */
            status &= 0xFFFF ^ (CO_CAN_ERRTX_BUS_OFF |
                                CO_CAN_ERRRX_WARNING | CO_CAN_ERRRX_PASSIVE |
                                CO_CAN_ERRTX_WARNING | CO_CAN_ERRTX_PASSIVE);

            /* rx bus warning or passive */
            if (rxErrors >= 128) {
                status |= CO_CAN_ERRRX_WARNING | CO_CAN_ERRRX_PASSIVE;
            } else if (rxErrors >= 96) {
                status |= CO_CAN_ERRRX_WARNING;
            }

            /* tx bus warning or passive */
            if (txErrors >= 128) {
                status |= CO_CAN_ERRTX_WARNING | CO_CAN_ERRTX_PASSIVE;
            } else if (rxErrors >= 96) {
                status |= CO_CAN_ERRTX_WARNING;
            }

            /* if not tx passive clear also overflow */
            if ((status & CO_CAN_ERRTX_PASSIVE) == 0) {
                status &= 0xFFFF ^ CO_CAN_ERRTX_OVERFLOW;
            }
        }

        if (overflow != 0) {
            /* CAN RX bus overflow */
            status |= CO_CAN_ERRRX_OVERFLOW;
        }

        CANmodule->CANerrorStatus = status;
    }
}


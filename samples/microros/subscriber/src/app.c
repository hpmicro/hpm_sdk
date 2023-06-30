/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#define RCCHECK(fn) \
{ rcl_ret_t temp_rc = fn; if (temp_rc != RCL_RET_OK) { printf("Failed status on line %d: %d. Aborting.\n", __LINE__, (int)temp_rc); vTaskDelete(NULL); }}
#define RCSOFTCHECK(fn) \
{ rcl_ret_t temp_rc = fn; if (temp_rc != RCL_RET_OK) { printf("Failed status on line %d: %d. Continuing.\n", __LINE__, (int)temp_rc); }}

rcl_subscription_t subscriber;
std_msgs__msg__Int32 msg;

void subscription_callback(const void *msgin)
{
	const std_msgs__msg__Int32 *msg = (const std_msgs__msg__Int32 *)msgin;
	printf("Received: %d\n", msg->data);
}

void appMain(void *arg)
{
	rcl_allocator_t allocator = rcl_get_default_allocator();
	rclc_support_t support;

	RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

	rcl_node_t node;
	RCCHECK(rclc_node_init_default(&node, "subscriber", "", &support));

	RCCHECK(rclc_subscription_init_default(
		&subscriber,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
		"int32_subscriber"));

	rclc_executor_t executor;
	RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
	RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA));

	while (1) {
		rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
		vTaskDelay(100);
	}

	RCCHECK(rcl_subscription_fini(&subscriber, &node));
	RCCHECK(rcl_node_fini(&node));

	vTaskDelete(NULL);
}

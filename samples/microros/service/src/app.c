/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <allocators.h>
#include <rcl/rcl.h>
#include <rcl_action/rcl_action.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/int32.h>
#include "example_interfaces/srv/add_two_ints.h"
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>
#include <stdio.h>

#define RCCHECK(fn) \
{ rcl_ret_t temp_rc = fn; if (temp_rc != RCL_RET_OK) { printf("Failed status on line %d: %d. Aborting.\n", __LINE__, (int)temp_rc); }}
#define RCSOFTCHECK(fn) \
{ rcl_ret_t temp_rc = fn; if (temp_rc != RCL_RET_OK) { printf("Failed status on line %d: %d. Continuing.\n", __LINE__, (int)temp_rc); }}

void service_callback(const void *req, void *res)
{
  example_interfaces__srv__AddTwoInts_Request *req_in = (example_interfaces__srv__AddTwoInts_Request *) req;
  example_interfaces__srv__AddTwoInts_Response *res_in = (example_interfaces__srv__AddTwoInts_Response *) res;

  printf("Service request value: %d + %d.\n", (int) req_in->a, (int) req_in->b);

  res_in->sum = req_in->a + req_in->b;
}

void appMain(void *arg)
{
    rcl_allocator_t allocator = rcl_get_default_allocator();

    rclc_support_t support;
    RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

    rcl_node_t node;
    RCCHECK(rclc_node_init_default(&node, "service", "", &support));

    rcl_service_t service;
    RCCHECK(rclc_service_init_default(&service, &node, ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts), "/addtwoints"));

    rclc_executor_t executor;
    RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));

    example_interfaces__srv__AddTwoInts_Response res;
    example_interfaces__srv__AddTwoInts_Request req;
    RCCHECK(rclc_executor_add_service(&executor, &service, &req, &res, service_callback));

    rclc_executor_spin(&executor);

    RCCHECK(rcl_service_fini(&service, &node));
    RCCHECK(rcl_node_fini(&node));
}

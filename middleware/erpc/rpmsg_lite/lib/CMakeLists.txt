# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(include)
sdk_inc_ifndef(CONFIG_FREERTOS include/environment/bm)
sdk_inc_ifdef(CONFIG_FREERTOS include/environment/freertos)
sdk_inc(include/platform/hpm6xxx)

sdk_src(common/llist.c)

sdk_src_ifndef(CONFIG_FREERTOS rpmsg_lite/porting/environment/rpmsg_env_bm.c)
sdk_src_ifdef(CONFIG_FREERTOS rpmsg_lite/porting/environment/rpmsg_env_freertos.c)
sdk_src(rpmsg_lite/porting/platform/hpm6xxx/rpmsg_platform.c)
sdk_src(rpmsg_lite/rpmsg_lite.c)
sdk_src(rpmsg_lite/rpmsg_ns.c)
sdk_src(rpmsg_lite/rpmsg_queue.c)

sdk_src(virtio/virtqueue.c)
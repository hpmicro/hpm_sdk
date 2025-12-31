.. _microros_libmicroros:

**libmicroros**
============================================

- **libmicroros.a** 是microros的核心代码库，包含ros api、ros middleware interface以及DDS的实现。

- 目前为GCC toolchain以及andes toolchain生成了对应的静态库，用户可以根据需要选择生成对应的工程。

.. note::

   - 目前microros链接的静态库是由GCC 13.1.0编译，Andes Toolchain v540编译

- 静态库生成配置文件如下:
   ROS2 节点数量为6个，发布者数量为4个，订阅者数量为4个，服务数量为2个，客户端数量为2个，历史缓存数量为2个。

.. code-block:: json

   {
      "names": {
         "rmw_microxrcedds": {
               "cmake-args": [
                  "-DRMW_UXRCE_MAX_NODES=6",
                  "-DRMW_UXRCE_MAX_PUBLISHERS=4",
                  "-DRMW_UXRCE_MAX_SUBSCRIPTIONS=4",
                  "-DRMW_UXRCE_MAX_SERVICES=2",
                  "-DRMW_UXRCE_MAX_CLIENTS=2",
                  "-DRMW_UXRCE_MAX_HISTORY=2",
               ]
         }
      }
   }

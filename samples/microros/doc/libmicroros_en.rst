.. _microros_libmicroros:

**libmicroros**
============================================

- **libmicroros.a** is the core code library of micro-ROS, which includes the ROS API, ROS middleware interface, and the implementation of DDS.

- Currently, corresponding static libraries have been generated for the GCC toolchain and Andes toolchain. Users can choose to generate the corresponding project as needed.

.. note::

   - Currently, the static library linked by microros is compiled by GCC 13.1.0 and Andes Toolchain v540.

- The configuration file for static library generation is as follows:
   ROS 2 nodes: 6, publishers: 4, subscribers: 4, services: 2, clients: 2, history caches: 2.

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
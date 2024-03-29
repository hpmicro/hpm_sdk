if(${CONFIG_ECLIPSE_THREADX_NETXDUO_UTILITY_IPERF})
    sdk_src(${CMAKE_CURRENT_LIST_DIR}/iperf/nx_iperf.c)
    sdk_inc(${CMAKE_CURRENT_LIST_DIR}/iperf)
endif()
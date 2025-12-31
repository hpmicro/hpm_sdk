if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/common")
	set(common_dir "${CMAKE_CURRENT_SOURCE_DIR}/common")
else()
	set(common_dir "../../common")
endif()

set(COMPONENT_SRCS
	"slave_control.c"
	"${common_dir}/proto/esp_hosted_rpc.pb-c.c"
	"${common_dir}/utils/esp_hosted_cli.c"
	"protocomm_pserial.c"
	"esp_hosted_coprocessor.c"
	"slave_bt.c"
	"mempool.c"
	"stats.c"
	"mempool_ll.c"
	"host_power_save.c"
	"lwip_filter.c"
)

set(COMPONENT_ADD_INCLUDEDIRS
	"."
	"${common_dir}"
	"${common_dir}/log"
	"${common_dir}/proto"
	"${common_dir}/rpc"
	"${common_dir}/transport"
)

# Select BT UART code based on IDF Target
if(CONFIG_IDF_TARGET_ESP32)
    list(APPEND COMPONENT_SRCS slave_bt_uart_esp32.c)
elseif(CONFIG_IDF_TARGET_ESP32C3 OR CONFIG_IDF_TARGET_ESP32S3)
    list(APPEND COMPONENT_SRCS slave_bt_uart_esp32c3_s3.c)
else()
    list(APPEND COMPONENT_SRCS slave_bt_uart_esp32xx.c)
endif()

if(CONFIG_ESP_SDIO_HOST_INTERFACE)
    list(APPEND COMPONENT_SRCS sdio_slave_api.c)
elseif(CONFIG_ESP_SPI_HOST_INTERFACE)
    list(APPEND COMPONENT_SRCS spi_slave_api.c)
elseif(CONFIG_ESP_SPI_HD_MODE)
    list(APPEND COMPONENT_SRCS spi_hd_slave_api.c)
else(CONFIG_ESP_UART_HOST_INTERFACE)
    list(APPEND COMPONENT_SRCS uart_slave_api.c)
endif()

if(CONFIG_ESP_HOSTED_COPROCESSOR_EXAMPLE_MQTT)
    list(APPEND COMPONENT_SRCS mqtt_example.c)
endif()

if(CONFIG_ESP_HOSTED_COPROCESSOR_EXAMPLE_HTTP_CLIENT)
    list(APPEND COMPONENT_SRCS http_req.c)
endif()

# cli
list(APPEND COMPONENT_ADD_INCLUDEDIRS "${common_dir}/utils")
list(APPEND COMPONENT_SRCS "${common_dir}/utils/esp_hosted_cli.c")


register_component()

# Add directory of protocomm_priv.h to include paths
idf_component_get_property(protocomm_dir protocomm COMPONENT_DIR)
target_include_directories(${COMPONENT_LIB} PRIVATE "${protocomm_dir}/src/common")

# Add linker options to wrap esp_wifi_init function
target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=esp_wifi_init")

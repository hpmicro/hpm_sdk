#include "board.h"
#include "hpm_gpio_drv.h"
#include "hpm_ipc_event_mgr.h"
#include "hpm_pmp_drv.h"
#include "erpc_matrix_multiply.h"
#include "erpc_matrix_multiply_server.h"
#include "erpc_server_setup.h"
#include "rpmsg_lite.h"

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/url.h"
#include "mp4.h"
#include <stdio.h>
#include "h264_stream.h"
#include "app.h"


void erpc_switch_lightled(void)
{
    board_led_toggle();
}

/*!
 * @brief core1 send deocded frame data to core0
 */
void erpc_get_decoded_frame(int32_t * buf, int32_t * len)
{
    *buf = app_config.output_addr;
    if (app_config.got_picture) {
        *len = 1;
        app_config.flag_data_output = 1;
        app_config.got_picture = 0;
    } else {
        *len = 0;
    }
}

/*!
 * @brief core0 transfer stream data to core1
 */
void erpc_transfer_stream_data(int32_t buf, int32_t len)
{
    app_config.h264_data = (unsigned char* )buf;
    app_config.h264_data_len = len;
    app_config.flag_decode = 1;
}

/*!
 * @brief core1 send undealed data size to core0
 */
int32_t erpc_get_undealed_data_size(void)
{
    return app_config.h264_data_len;
}
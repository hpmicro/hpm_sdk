#include "measure.h"
#include "mp4.h"

#include "hpm_mchtmr_drv.h"

extern ff_measure_context_t s_ffMeasureContext;
extern ff_measure_sum_t s_ffMeasureSum;

static uint64_t time_tmp = 0;
static uint64_t time_measure_done(void)
{
    return (mchtmr_get_count(HPM_MCHTMR) - time_tmp) * 42;
}

void time_measure_start(void)
{
    time_tmp = mchtmr_get_count(HPM_MCHTMR);
}

void ff_time_measure_utility(char type)
{
#if MP4_FF_TIME_ENABLE == 1

    switch (type) {
    case kFfTimeType_Start:
        time_measure_start();
        memset(&s_ffMeasureContext, 0, sizeof(s_ffMeasureContext));
    break;
    case kFfTimeType_ReadFrame:
        s_ffMeasureContext.readFrame_ns = time_measure_done();
    break;
    case kFfTimeType_DecodeAudio:
        s_ffMeasureContext.decodeAudio_ns = time_measure_done();
    break;
    case kFfTimeType_DecodeVideo:
        s_ffMeasureContext.decodeVideo_ns = time_measure_done();
    break;
    case kFfTimeType_TransStart:
        s_ffMeasureContext.decodeVideo_TransStart = time_measure_done();
    break;
    case kFfTimeType_TransEnd:
        s_ffMeasureContext.decodeVideo_TransEnd = time_measure_done();
        s_ffMeasureSum.decodeVideo_TransEnd += (s_ffMeasureContext.decodeVideo_TransEnd -
                                            s_ffMeasureContext.decodeVideo_TransStart);
    break;
    case kFfTimeType_Pdma:
        s_ffMeasureContext.decodeVideo_Pdma = time_measure_done();
        s_ffMeasureSum.decodeVideo_Pdma += (s_ffMeasureContext.decodeVideo_Pdma -
                                            s_ffMeasureContext.decodeVideo_TransEnd);
    break;
    }

#else
    /*Do nothing*/
#endif
}
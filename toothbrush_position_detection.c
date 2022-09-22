#include "toothbrush_position_detection.h"
#include "ln_alg_mem.h"
#include "ln_math_types.h"
#include "ln_log.h"
#include "tpd_detection.h"
#include <stdlib.h>

#ifdef LN_DEBUG
#include "ln_debug.h"
#endif

tpdLogCallback_t lnLog = NULL;

#define TPD_MOTION_BUF_LENGTH (128)
#define TPD_PRS_THRESHOLD     (1)
#define TPD_ACC_SCALE_FACTOR  (1.0f / 8192.0f)     // (1.0f / 4096.0f) (1.0f / 8192.0f)
#define TPD_GYR_SCALE_FACTOR  (1000.0f / 32767.0f) // (2000.0f / 32767.0f) (1000.0f / 32767.0f)


/** tpdWritePosition is the default func point to tpdNotify. */
tpdGetPositionCallback_t tpdNotify = (tpdGetPositionCallback_t)tpdWritePosition;

static uint8_t k_is_sample_rate_set = 0u;
static uint8_t k_is_user_info_set   = 0u;
static uint8_t k_is_inited          = 0u;

void tpdAlgInit(void)
{
    tpdPosition_t* position = tpdTransferPosition();
    if (!k_is_sample_rate_set) {
        position->log = LOG_INIT_ERROR;
        tpdNotify(*position);
        LN_LOG(LN_LOG_ERROR, "TPD >>> Sample rate not Set.");
        return;
    }
    else if (!k_is_user_info_set)
    {
        position->log = LOG_INIT_ERROR;
        tpdNotify(*position);
        LN_LOG(LN_LOG_ERROR, "TPD >>> User info not Set.");
        return;
    }

    tpdDetectInit();

    k_is_inited = 1u;

    LN_LOG(LN_LOG_INFO, "TPD >>> Initialized.\n");

    return;
}

void tpdAlgUninit(void)
{
    k_is_sample_rate_set = 0u;
    k_is_user_info_set = 0u;
    k_is_inited = 0u;

    tpdDetectUninit();

    return;
}

tpdLog_t tpdSetSampleRate(tpdSigSource_t source, int16_t sample_rate, tpdMotionSensorId_t sensor_id)
{
    if (sample_rate < 100)
    {
        LN_LOG(LN_LOG_ERROR, "TPD >>> Sample rate is too low.");
        return LOG_SAMPLE_RATE_ERROR;
    }

    switch (sensor_id)
    {
    case ICM40608:
        break;
    case ICMxxxxx:
        break;
    default:
        LN_LOG(LN_LOG_ERROR, "TPD >>> SensorID[0x%4x] is not supported.\n", sensor_id);
        return LOG_SENSOR_ERROR;
    }

    tpdSetDataInterval(sample_rate);
    k_is_sample_rate_set = 1u;

    return LOG_SUCCESS;
}

tpdLog_t tpdSetUserInfo(tpdArea_t *areas, int32_t *area_durations, int32_t length)
{
    if (length > 6)
    {
        LN_LOG(LN_LOG_ERROR, "TPD >>> Too many areas.\n");
        return LOG_AREA_ERROR;
    }

    tpdSetAreaLink(areas, area_durations, length);

    k_is_user_info_set = 1u;

    return LOG_SUCCESS;
}

__weak void tpdAlgPushData(tpdSignal_t *input)
{
    // LN_LOG(LN_LOG_ERROR, "tpdAlgPushData __weak.\n");
}

void tpdAlgProcess(tpdSignal_t *input)
{
    tpdAlgPushData(input);

    /** If not initialized, notify algorithm is stopped. */
    tpdPosition_t* position;
    if (k_is_inited == 0u)
    {
        position = tpdTransferPosition();
        position->log = LOG_STOPPED;
        tpdNotify(*position);
        LN_LOG(LN_LOG_WARNING, "TPD >>> Not Initialized.\n");
    }

    /** Fill bufs. */
    tpdSigSource_t source = input->sensor_type;
    switch (source)
    {
    case ALG_SOURCE_MOTION:
        for (int32_t i = 0; i < input->sample_length; ++i)
        {
            float32_t motion_data[6] =
            {
                (float32_t)(input->sig_t.motion_t.acc_x[i]) * TPD_ACC_SCALE_FACTOR,
                (float32_t)(input->sig_t.motion_t.acc_y[i]) * TPD_ACC_SCALE_FACTOR,
                (float32_t)(input->sig_t.motion_t.acc_z[i]) * TPD_ACC_SCALE_FACTOR,
                (float32_t)(input->sig_t.motion_t.gyr_x[i]) * TPD_GYR_SCALE_FACTOR,
                (float32_t)(input->sig_t.motion_t.gyr_y[i]) * TPD_GYR_SCALE_FACTOR,
                (float32_t)(input->sig_t.motion_t.gyr_z[i]) * TPD_GYR_SCALE_FACTOR, 
            };
            tpdDetect(motion_data, input->time_stamp[i]);
        }
        break;
    case ALG_SOURCE_PRESS:
        {
            tpdAlgStatus_t status = ALG_STATUS_NORMAL;
            position = tpdTransferPosition();
            for (int32_t i = 0; i < input->sample_length; ++i)
            {
                switch (input->sig_t.signal[i])
                {
                case PRESS_LEVEL_0:
                    status = ALG_STATUS_LOW_PRESS;
                    break;
                case PRESS_LEVEL_2:
                    status = ALG_STATUS_OVER_PRESS;
                    break;
                default:
                    break;
                }

                if (position->status != status)
                {
                    position->status = status;
                    tpdNotify(*position);
                }
            }
        }
        break;
    default:
        break;
    }

    return;
}

uint32_t tpdGetAlgVersion(void)
{
    return 0x00010000;  /**<< v.0.1.0.0. */
}

uint32_t tpdGetAlgCapability(void)
{
    return 0x0000060C;  /**<< Support 6 areas and 12 surfaces.*/
}

void tpdWritePosition(tpdPosition_t position)
{
    LN_LOG(LN_LOG_DEBUG, "TPD >>> position: area = 0x%04x, surface = 0x%04x, log = %2d, status = %2d, time_stamp = %7d msc.\n",
          position.area, position.surface, position.log, position.status, position.time_stamp);
#ifdef LN_DEBUG
    lnDebugTracker_t *TRACKER = lnDebugGetTracker();
    TRACKER->position_member_buf[TRACKER->position_member_buf_length++] = (int32_t)position.area;
    TRACKER->position_member_buf[TRACKER->position_member_buf_length++] = (int32_t)position.surface;
    TRACKER->position_member_buf[TRACKER->position_member_buf_length++] = (int32_t)position.log;
    TRACKER->position_member_buf[TRACKER->position_member_buf_length++] = (int32_t)position.status;
    TRACKER->position_member_buf[TRACKER->position_member_buf_length++] = (int32_t)position.time_stamp;  /**< !Check. */
#endif
    return;
}

void tpdGetPositionRegister(tpdGetPositionCallback_t func)
{
    if (func != NULL)
    {
        tpdNotify = func;
    }
    return;
}

void tpdMallocRegister(tpdMallocCallback_t func)
{
    if (func != NULL)
    {
        lnAlgMalloc = func;
    }
    return;
}

void tpdFreeRegister(tpdFreeCallback_t func)
{
    if (func != NULL)
    {
        lnAlgFree = func;
    }
    return;
}

void tpdLogRegister(tpdLogCallback_t func)
{
    if (func != NULL)
    {
        lnLog = func;
    }
    return;
}

#include "tpd_detection.h"
#include "ln_attitude_angle.h"
#include "ln_array.h"
#include "ln_alg_mem.h"
#include "ln_log.h"
#include "tpd_model.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef LN_DEBUG
#include "ln_debug.h"
#endif

#define TPD_NUM_AREA          6
#define TPD_NUM_SURFACE       3
#define TPD_MOTION_BUF_LENGTH 100
#define TPD_AVG_LENGTH        20
#define TPD_NUM_FEATS_SURFACE 8
#define TPD_NUM_FEATS_AREA    8

/**
 * @brief This structure defines fixed brush duration in 6 consecutive area (only for V1.x). 
 */
typedef struct tpdAreaLink
{
    //struct tpdAreaLink_t *next;  /**< The next free block in the list. */ //gxh FIXME
    struct tpdAreaLink *next;
    tpdArea_t area;
    int32_t preset_area_duration;
    int32_t area_duration;
} tpdAreaLink_t;

/** These variables must be set before tpdDetectInit. */
static tpdAreaLink_t *k_area_link;
static int32_t k_total_duration;
static int32_t k_data_interval;

/** These variables set in tpdDetectInit. */
static float32_t *k_acc_x_buf;
static float32_t *k_acc_y_buf;
static float32_t *k_acc_z_buf;
static float32_t *k_gyr_x_buf;
static float32_t *k_gyr_y_buf;
static float32_t *k_gyr_z_buf;
static float32_t *k_pitch_buf;
static float32_t *k_roll_buf;
static float32_t *k_yaw_buf;

/**< moving-average filter for raw acc data. */
static float32_t k_sum_buf[6];                   /**< 6 + 3                           */
static float32_t k_reg_buf[TPD_AVG_LENGTH * 6];  /**< Moving-average register.        */
static uint8_t k_pos_buf[6];                     /**< Last data position in register. */

static int16_t k_n_fill;
static int32_t k_duration;
static int32_t k_last_duration;
static tpdPosition_t k_position = 
{
    .area       = AREA_LEFT_UPPER_JAW,
    .surface    = SURFACE_OUTER,
    .log        = LOG_SUCCESS,
    .status     = ALG_STATUS_NORMAL,
    .time_stamp = 0
};

static int32_t k_debug_time_stamp;

tpdPosition_t* tpdTransferPosition(void)
{
    return &k_position;
}


void tpdSetDataInterval(int16_t sample_rate)
{
    k_data_interval = (int32_t)(1000 / sample_rate);
    LN_LOG(LN_LOG_INFO, "TPD >>> Configuration: data interval under %d Hz sample rate is %u msc.\n", sample_rate, k_data_interval);
    return;
}


void tpdSetAreaLink(tpdArea_t *areas, int32_t *area_durations, int32_t length)
{
    k_area_link = (tpdAreaLink_t*)lnAlgMalloc(sizeof(tpdAreaLink_t));
    tpdAreaLink_t *area_link = k_area_link;

    for (uint32_t i = 0u; i < TPD_NUM_AREA; ++i)
    {
        tpdAreaLink_t *area_link_next         = (tpdAreaLink_t*)lnAlgMalloc(sizeof(tpdAreaLink_t));
        area_link_next->area                  = areas[i];
        area_link_next->preset_area_duration  = area_durations[i];
        area_link_next->area_duration         = 0;
        area_link_next->next                  = NULL;
        area_link->next                       = area_link_next;
        area_link                             = area_link->next;
        k_total_duration                      = k_total_duration + area_durations[i];
        LN_LOG(LN_LOG_INFO, "TPD >>> User info: area[0x%04x] duration is set as %u msc.\n", area_link->area, area_link_next->preset_area_duration);
    }

    return;
}


static tpdArea_t tpdGetArea()
{
    tpdAreaLink_t *area_link = k_area_link->next;  /** Ignore head node. */

    while (area_link->area_duration >= area_link->preset_area_duration)
    {
        if (area_link->next == NULL)
        {
            break;
        }
        area_link = area_link->next;
    }

    area_link->area_duration = area_link->area_duration + (k_duration - k_last_duration);

    if (area_link->area_duration == area_link->preset_area_duration)
    {
        if (area_link->next != NULL)
        {
            area_link = area_link->next;
        }
    }
    tpdArea_t area = area_link->area;

    return area;
}


static tpdSurface_t tpdGetSurface(tpdArea_t area)
{
    /** Extract features. */
    float32_t f[TPD_NUM_FEATS_SURFACE];
    int32_t f_length = 0;
    tpdFeat_t feats[TPD_NUM_FEATS_SURFACE];
    float32_t *temp = (float32_t *)lnAlgMalloc(sizeof(float32_t) * TPD_MOTION_BUF_LENGTH);

    /** 'acc_x__quantile__q_0.9' */
    f[f_length++] = F1(k_acc_x_buf, TPD_MOTION_BUF_LENGTH, 0.1f);

    /** 'acc_xmy__quantile__q_0.7' */
    for (int32_t i = 0; i < TPD_MOTION_BUF_LENGTH; ++i) { temp[i] = k_acc_x_buf[i] * k_acc_y_buf[i]; }
    lnArrSort(temp, TPD_MOTION_BUF_LENGTH);
    f[f_length++] = F2(temp, TPD_MOTION_BUF_LENGTH, 0.7f);

    /** 'acc_ymz__root_mean_square' */
    for (int32_t i = 0; i < TPD_MOTION_BUF_LENGTH; ++i) { temp[i] = k_acc_y_buf[i] * k_acc_z_buf[i]; }
    f[f_length++] = F3(temp, TPD_MOTION_BUF_LENGTH);

    /** 'acc_x__agg_linear_trend__attr_"slope"__chunk_len_50__f_agg_"max"' */
    float32_t slope;
    F4(k_acc_x_buf, TPD_MOTION_BUF_LENGTH, 50, 2, NULL, NULL, &slope);
    f[f_length++] = slope;

    /** 'acc_xmz__autocorrelation__lag_7' */
    for (int32_t i = 0; i < TPD_MOTION_BUF_LENGTH; ++i) { temp[i] = k_acc_x_buf[i] * k_acc_z_buf[i]; }
    f[f_length++] = F5(temp, TPD_MOTION_BUF_LENGTH, 7);

    /** 'acc_xmy__minimum' */
    for (int32_t i = 0; i < TPD_MOTION_BUF_LENGTH; ++i) { temp[i] = k_acc_x_buf[i] * k_acc_y_buf[i]; }
    F6(temp, TPD_MOTION_BUF_LENGTH);
    f[f_length++] = temp[0];

    /** 'gyr_ymz__quantile__q_0.1' */
    for (int32_t i = 0; i < TPD_MOTION_BUF_LENGTH; ++i) { temp[i] = k_gyr_y_buf[i] * k_gyr_z_buf[i]; }
    F6(temp, TPD_MOTION_BUF_LENGTH);
    f[f_length++] = F7(temp, TPD_MOTION_BUF_LENGTH, 0.1f);

    /** 'acc_z__change_quantiles__f_agg_"var"__isabs_False__qh_0.8__ql_0.6' */
    for (int32_t i = 0; i < TPD_MOTION_BUF_LENGTH; ++i) { temp[i] = k_acc_z_buf[i]; }
    F6(temp, TPD_MOTION_BUF_LENGTH);
    float32_t qh = F7(temp, TPD_MOTION_BUF_LENGTH, 0.8f);
    float32_t ql = F7(temp, TPD_MOTION_BUF_LENGTH, 0.6f);
    f[f_length++] = F8(k_acc_z_buf, TPD_MOTION_BUF_LENGTH, qh, ql, false, 1);

    /** Transorm. */
    for (int32_t i = 0; i < TPD_NUM_FEATS_SURFACE; ++i)
    {
        feats[i].fvalue = f[i];
        // LN_LOG(LN_LOG_DEBUG, "TPD >>> Feats[%d].fvalue: %f\n", i, feats[i].fvalue);
    };

    /**< 模型结果. */
    float prob = tpdPredictSurface(feats);
    int32_t pred = 0;
    if (prob > 0.5)
    {
        pred = 1;
    }

    /* Map surface for different areas. */
    tpdSurface_t surface;
    if (pred == 0)
    {
        if ((area == AREA_RIGHT_UPPER_JAW) || (area == AREA_RIGHT_LOWER_JAW))
        {
            surface = SURFACE_INNER;
        }
        else
        {
            surface = SURFACE_OUTER;
        }
    }
    else
    {
        if ((area == AREA_RIGHT_UPPER_JAW) || (area == AREA_RIGHT_LOWER_JAW))
        {
            surface = SURFACE_OUTER;
        }
        else
        {
            surface = SURFACE_INNER;
        }
    }

    /**< 返回结果. */
    lnAlgFree(temp);

#ifdef LN_DEBUG
    lnDebugTracker_t *TRACKER = lnDebugGetTracker();

    for (int32_t i = 0; i < TPD_NUM_FEATS_SURFACE; ++i)
    {
        TRACKER->surface_buf[TRACKER->surface_buf_length++] = f[i];
    }
    TRACKER->surface_buf[TRACKER->surface_buf_length++] = prob;
    TRACKER->surface_buf[TRACKER->surface_buf_length++] = pred;
    TRACKER->surface_buf[TRACKER->surface_buf_length++] = (float32_t)surface;
    TRACKER->surface_buf[TRACKER->surface_buf_length++] = k_debug_time_stamp;
#endif

    // return surface;
    return SURFACE_OUTER;
}


static tpdAlgStatus_t tpdPostProcess(tpdArea_t area, tpdSurface_t surface, float32_t *pitches, float32_t *rolls, float32_t *yaws, int32_t length)
{
    
    tpdAlgStatus_t status = ALG_STATUS_NORMAL;

    float32_t *temp = (float32_t*)lnAlgMalloc(sizeof(float32_t) * length);

    /** These conditions are used for LEFT and RIGHT areas. */
    if ((area == AREA_LEFT_UPPER_JAW) || (area == AREA_RIGHT_UPPER_JAW) || (area == AREA_LEFT_LOWER_JAW) || (area == AREA_RIGHT_LOWER_JAW))
    {
        
        /** Condition 1: the roll should be not fall in [-180, -150]/[150, 180], or [-30, 0]/[0, 30]. */
        for (int32_t i = 0; i < length; ++i) { temp[i] = fabs(rolls[i]); }
        lnArrSort(temp, length);
        
        float32_t q = lnArrQuantile(temp, (uint32_t)length, 0.9);
        if ((q >= 150) || (q <= 30))
        {
            LN_LOG(LN_LOG_DEBUG, "TPD >>> Invalid position, roll = %f, time_stamp = %d.\n", q, k_position.time_stamp);
            status = ALG_STASUS_INVALID_POSITION;
        }

        /** Condition 2: the pitch should be not fall in [-120, -60], and [60, 120]. */
        for (int32_t i = 0; i < length; ++i) { temp[i] = pitches[i]; }
        lnArrSort(temp, length);
        
        float32_t q_lower = lnArrQuantile(temp, (uint32_t)length, 0.1f);
        float32_t q_upper = lnArrQuantile(temp, (uint32_t)length, 0.9f);
        if (((q_lower >= 60) && (q_upper <= 120)) || ((q_lower >= -120) && (q_upper <= -60)))
        {
            LN_LOG(LN_LOG_DEBUG, "TPD >>> Invalid position, pitch = [%f, %f], time_stamp = %d.\n", q_lower, q_upper, k_position.time_stamp);
            status = ALG_STASUS_INVALID_POSITION;
        }
    }

    lnAlgFree(temp);
    
    
    return status;
}


void tpdDetectInit(void)
{
    k_acc_x_buf = (float32_t*)lnAlgMalloc(sizeof(float32_t) * TPD_MOTION_BUF_LENGTH);
    k_acc_y_buf = (float32_t*)lnAlgMalloc(sizeof(float32_t) * TPD_MOTION_BUF_LENGTH);
    k_acc_z_buf = (float32_t*)lnAlgMalloc(sizeof(float32_t) * TPD_MOTION_BUF_LENGTH);
    k_gyr_x_buf = (float32_t*)lnAlgMalloc(sizeof(float32_t) * TPD_MOTION_BUF_LENGTH);
    k_gyr_y_buf = (float32_t*)lnAlgMalloc(sizeof(float32_t) * TPD_MOTION_BUF_LENGTH);
    k_gyr_z_buf = (float32_t*)lnAlgMalloc(sizeof(float32_t) * TPD_MOTION_BUF_LENGTH);
    k_pitch_buf = (float32_t*)lnAlgMalloc(sizeof(float32_t) * TPD_MOTION_BUF_LENGTH);
    k_roll_buf  = (float32_t*)lnAlgMalloc(sizeof(float32_t) * TPD_MOTION_BUF_LENGTH);
    k_yaw_buf   = (float32_t*)lnAlgMalloc(sizeof(float32_t) * TPD_MOTION_BUF_LENGTH);
    memset(k_sum_buf, 0, sizeof(k_sum_buf));
    memset(k_pos_buf, 0, sizeof(k_pos_buf));
    memset(k_reg_buf, 0, sizeof(k_reg_buf));
    k_n_fill    = 0;
    k_duration  = 0;
    k_last_duration = 0;

    k_debug_time_stamp = 0;

    float32_t demo[3] = {0.133789, -0.604248, -0.787354};
    lnAttitudeAngleInit(demo);

    return;
}


void tpdDetectUninit(void)
{   
    lnAlgFree(k_acc_x_buf);
    lnAlgFree(k_acc_y_buf);
    lnAlgFree(k_acc_z_buf);
    lnAlgFree(k_gyr_x_buf);
    lnAlgFree(k_gyr_y_buf);
    lnAlgFree(k_gyr_z_buf);
    lnAlgFree(k_pitch_buf);
    lnAlgFree(k_roll_buf);
    lnAlgFree(k_yaw_buf);

    tpdAreaLink_t *area_link = k_area_link;
    while (area_link != NULL)
    {
        tpdAreaLink_t *area_link_next = area_link->next;
        lnAlgFree(area_link);
        area_link = area_link_next;
    }
    k_n_fill         = 0;
    k_duration       = 0;
    k_data_interval  = 0;
    k_total_duration = 0;

    k_position.log  = LOG_STOPPED;

    lnAttitudeAngleUninit();

    return;
}


/**
 * @brief This function detects tooth brush postion.
 * 
 * @param motion_data 
 * @param time_stamp  msc
 */
void tpdDetect(float32_t motion_data[6], int32_t time_stamp)
{
    /** Moving update. */
    if (k_n_fill == TPD_MOTION_BUF_LENGTH)
    {
        k_n_fill = TPD_MOTION_BUF_LENGTH / 2;

        lnArrRoll(k_acc_x_buf, TPD_MOTION_BUF_LENGTH, (int16_t)(TPD_MOTION_BUF_LENGTH / 2));
        lnArrRoll(k_acc_y_buf, TPD_MOTION_BUF_LENGTH, (int16_t)(TPD_MOTION_BUF_LENGTH / 2));
        lnArrRoll(k_acc_z_buf, TPD_MOTION_BUF_LENGTH, (int16_t)(TPD_MOTION_BUF_LENGTH / 2));
        lnArrRoll(k_gyr_x_buf, TPD_MOTION_BUF_LENGTH, (int16_t)(TPD_MOTION_BUF_LENGTH / 2));
        lnArrRoll(k_gyr_y_buf, TPD_MOTION_BUF_LENGTH, (int16_t)(TPD_MOTION_BUF_LENGTH / 2));
        lnArrRoll(k_gyr_z_buf, TPD_MOTION_BUF_LENGTH, (int16_t)(TPD_MOTION_BUF_LENGTH / 2));
        lnArrRoll(k_pitch_buf, TPD_MOTION_BUF_LENGTH, (int16_t)(TPD_MOTION_BUF_LENGTH / 2));
        lnArrRoll(k_roll_buf,  TPD_MOTION_BUF_LENGTH, (int16_t)(TPD_MOTION_BUF_LENGTH / 2));
        lnArrRoll(k_yaw_buf,   TPD_MOTION_BUF_LENGTH, (int16_t)(TPD_MOTION_BUF_LENGTH / 2));
    }

    /** Update raw data buffers. */
// #ifdef LN_DEBUG
//     k_acc_x_buf[k_n_fill] = motion_data[0];
//     k_acc_y_buf[k_n_fill] = motion_data[1];
//     k_acc_z_buf[k_n_fill] = motion_data[2];
//     k_gyr_x_buf[k_n_fill] = motion_data[3];
//     k_gyr_y_buf[k_n_fill] = motion_data[4];
//     k_gyr_z_buf[k_n_fill] = motion_data[5];
// #else
    k_acc_x_buf[k_n_fill] = lnArrMovingAvg(
        motion_data[0], &k_sum_buf[0], &k_reg_buf[0],                  &k_pos_buf[0], TPD_AVG_LENGTH);
    k_acc_y_buf[k_n_fill] = lnArrMovingAvg(
        motion_data[1], &k_sum_buf[1], &k_reg_buf[TPD_AVG_LENGTH],     &k_pos_buf[1], TPD_AVG_LENGTH);
    k_acc_z_buf[k_n_fill] = lnArrMovingAvg(
        motion_data[2], &k_sum_buf[2], &k_reg_buf[2 * TPD_AVG_LENGTH], &k_pos_buf[2], TPD_AVG_LENGTH);
    k_gyr_x_buf[k_n_fill] = lnArrMovingAvg(
        motion_data[3], &k_sum_buf[3], &k_reg_buf[3 * TPD_AVG_LENGTH], &k_pos_buf[3], TPD_AVG_LENGTH);
    k_gyr_y_buf[k_n_fill] = lnArrMovingAvg(
        motion_data[4], &k_sum_buf[4], &k_reg_buf[4 * TPD_AVG_LENGTH], &k_pos_buf[4], TPD_AVG_LENGTH);
    k_gyr_z_buf[k_n_fill] = lnArrMovingAvg(
        motion_data[5], &k_sum_buf[5], &k_reg_buf[5 * TPD_AVG_LENGTH], &k_pos_buf[5], TPD_AVG_LENGTH);
// #endif

    /** Get attitude angles. */
    lnAttitudeAngle_t angle = {.pitch = .0f, .roll = .0f, .yaw = .0f};
    float32_t motion_data_transformed[6] =
    { 
        k_acc_y_buf[k_n_fill], k_acc_x_buf[k_n_fill], k_acc_z_buf[k_n_fill],
        k_gyr_y_buf[k_n_fill], k_gyr_x_buf[k_n_fill], k_gyr_z_buf[k_n_fill]
    };
    lnAttitudeAngle(motion_data_transformed, .01f, &angle);

    /** Update attitude angle buffers. */
    k_pitch_buf[k_n_fill] = angle.pitch;
    k_roll_buf[k_n_fill]  = angle.roll;
    k_yaw_buf[k_n_fill]   = angle.yaw;


#ifdef LN_DEBUG
    lnDebugTracker_t *TRACKER = lnDebugGetTracker();

    TRACKER->motion_data_buf[TRACKER->motion_data_buf_length++] = motion_data[0];
    TRACKER->motion_data_buf[TRACKER->motion_data_buf_length++] = motion_data[1];
    TRACKER->motion_data_buf[TRACKER->motion_data_buf_length++] = motion_data[2];
    TRACKER->motion_data_buf[TRACKER->motion_data_buf_length++] = motion_data[3];
    TRACKER->motion_data_buf[TRACKER->motion_data_buf_length++] = motion_data[4];
    TRACKER->motion_data_buf[TRACKER->motion_data_buf_length++] = motion_data[5];

    TRACKER->attitude_angle_buf[TRACKER->attitude_angle_buf_length++] = angle.pitch;
    TRACKER->attitude_angle_buf[TRACKER->attitude_angle_buf_length++] = angle.roll;
    TRACKER->attitude_angle_buf[TRACKER->attitude_angle_buf_length++] = angle.yaw;
    TRACKER->attitude_angle_buf[TRACKER->attitude_angle_buf_length++] = (float)time_stamp;

    TRACKER->filt_data_buf[TRACKER->filt_data_buf_length++] = k_acc_x_buf[k_n_fill];
    TRACKER->filt_data_buf[TRACKER->filt_data_buf_length++] = k_acc_y_buf[k_n_fill];
    TRACKER->filt_data_buf[TRACKER->filt_data_buf_length++] = k_acc_z_buf[k_n_fill];
    TRACKER->filt_data_buf[TRACKER->filt_data_buf_length++] = k_gyr_x_buf[k_n_fill];
    TRACKER->filt_data_buf[TRACKER->filt_data_buf_length++] = k_gyr_y_buf[k_n_fill];
    TRACKER->filt_data_buf[TRACKER->filt_data_buf_length++] = k_gyr_z_buf[k_n_fill];
    TRACKER->filt_data_buf[TRACKER->filt_data_buf_length++] = k_pitch_buf[k_n_fill];
    TRACKER->filt_data_buf[TRACKER->filt_data_buf_length++] = k_roll_buf[k_n_fill];
    TRACKER->filt_data_buf[TRACKER->filt_data_buf_length++] = k_yaw_buf[k_n_fill];
#endif

    /** Update time stamp. The TPD >>> timing is determined by tpd_detection. */
    k_duration += k_data_interval;
    k_position.time_stamp = time_stamp;  /**< if the 1st data is 0, then the 2000th data is 19990 msc. */
    k_debug_time_stamp = time_stamp;

    if (k_duration % 1000 == 0)
    {
        LN_LOG(LN_LOG_INFO, "TPD >>> [1 second] position: area = 0x%04x, surface = 0x%04x, log = %2d, status = %2d, time_stamp = %7d msc.\n", k_position.area, k_position.surface, k_position.log, k_position.status, k_position.time_stamp);
    }

    if (++k_n_fill < TPD_MOTION_BUF_LENGTH)
    {
        return;
    }
    
    /** Get area. */
    tpdArea_t area = tpdGetArea();

    /** Get surface. */
    tpdSurface_t surface = tpdGetSurface(area);

    /** Filter impossible attitude angles. */
    tpdAlgStatus_t status  = tpdPostProcess(
        area, surface,
        &k_pitch_buf[TPD_MOTION_BUF_LENGTH / 2],
        &k_roll_buf[TPD_MOTION_BUF_LENGTH / 2],
        &k_yaw_buf[TPD_MOTION_BUF_LENGTH / 2],
        TPD_MOTION_BUF_LENGTH / 2);
    
#ifdef LN_DEBUG
    TRACKER->status_buf[TRACKER->status_buf_length++] = status;
    TRACKER->status_buf[TRACKER->status_buf_length++] = k_debug_time_stamp;
#endif

    k_last_duration = k_duration;

    /** Callback if one of [area, surface, status, log] changes. */
    if ((area != k_position.area) || (surface != k_position.surface) || (status != k_position.status))
    {
        k_position.area    = area;
        k_position.surface = surface;
        k_position.status  = status;

        if (k_duration >= k_total_duration)
        {
            /** TODO: Whether to call tpdDetectUninit(). */
            k_position.log = LOG_STOPPED;
        }

        tpdNotify(k_position);
    }

    return;
}

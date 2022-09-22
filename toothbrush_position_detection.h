#ifndef __TOOTHBRUSH_POSITION_H__
#define __TOOTHBRUSH_POSITION_H__

#include <stdint.h>

/**
 * @brief This enum defines algorithm library output status
 */
typedef enum
{
    LOG_SUCCESS    = 99,
    LOG_INIT_ERROR = 0,
    LOG_UNINIT_FAILED,      /**< Falied to stop algorithm. */
    LOG_STOPPED,            /**< Algorithm is stopped.     */
    LOG_MALLOC_FAILED,      /**< Malloc failed.            */
    LOG_FREE_FAILED,        /**< Free failed.              */
    LOG_SENSOR_ERROR,       /**< Unknown senesor           */
    LOG_SAMPLE_RATE_ERROR,  /**< Sample rate too low.      */
    LOG_MOTION_DATA_ERROR,  /**< Acceleration data error.  */
    LOG_AREA_ERROR,         /**< Unpermitted area setting. */
    LOG_ASSERT0,
} tpdLog_t;

/**
 * @brief This enum defines algorithm input signal source type
 */
typedef enum
{
    ALG_SOURCE_MOTION = 0x0001,  /**< The signal source is 6-axis.   */
    ALG_SOURCE_PRESS  = 0x0002   /**< The signal source is pressure. */
} tpdSigSource_t;

/**
 * @brief This structure defines the library input structure.
 */
typedef struct 
{
    tpdSigSource_t sensor_type;  /**< Signal source. */
    int32_t sample_length;
    int32_t *time_stamp;
    union
    {
        int32_t *signal;
        struct 
        {
            int32_t *acc_x;  /**< Acceleration X. */
            int32_t *acc_y;  /**< Acceleration Y. */
            int32_t *acc_z;  /**< Acceleration Z. */
            int32_t *gyr_x;  /**< Gyroscope X.    */
            int32_t *gyr_y;  /**< Gyroscope Y.    */
            int32_t *gyr_z;  /**< Gyroscope Z.    */
        } motion_t;
    } sig_t;
    // int32_t *serial_number;      /**< Check data loss only. */
} tpdSignal_t;

/**
 * @brief This enum defines algorithm library output status
 */
typedef enum tpdAlgStatus
{
    ALG_STATUS_OVER_FREQUENCY   = -1,   /**< The brush frequency is high.        */
    ALG_STATUS_OVER_AMPLITUEDE  = -2,   /**< The brush amplitude is large.       */
    ALG_STATUS_LOW_PRESS        = -3,   /**< The brush pressure is insufficient. */
    ALG_STATUS_OVER_PRESS       = -4,   /**< The brush pressure is large.        */
    ALG_STASUS_INVALID_POSITION = -5,   /**< The Position is NOT permitted.      */
    ALG_STATUS_NORMAL           = 0     /**< Normal.                             */
} tpdAlgStatus_t;

/**
 * @brief This enum defines toothbrush area.
 */
typedef enum
{
    AREA_LEFT_UPPER_JAW  = 0x0001,
    AREA_LEFT_LOWER_JAW  = 0x0002,
    AREA_UPPER_INCISORS  = 0x0004,
    AREA_LOWER_INCISORS  = 0x0008,
    AREA_RIGHT_UPPER_JAW = 0x0010,
    AREA_RIGHT_LOWER_JAW = 0x0020,
} tpdArea_t;

/**
 * @brief This enum defines toothbrush surface.
 */
typedef enum
{
    SURFACE_OUTER = 0x0001,
    SURFACE_TOP   = 0x0002,
    SURFACE_INNER = 0x0004
} tpdSurface_t;

/**
 * @brief This enum defines press level.
 */
typedef enum
{
    PRESS_LEVEL_0,  /**< Not detect press. */
    PRESS_LEVEL_1,  /**< Normal.           */
    PRESS_LEVEL_2,  /**< Large press.      */
    PRESS_LEVEL_3   /**< Recovering.       */
} tpdPress_t;

/**
 * @brief This structure defines algorithm output structure..
 */
typedef struct
{
    tpdArea_t area;
    tpdSurface_t surface;
    tpdLog_t log;
    tpdAlgStatus_t status;
    uint32_t time_stamp;    /**< msec. */
} tpdPosition_t;

/**
 * @brief Algorithm initialization. This func needs be call after 
 *        tpdSetSampleRate and tpdSetUserInfo.
 */
void tpdAlgInit(void);

/**
 * @brief Algorithm stop, release memry.
 */
void tpdAlgUninit(void);

/**
 * @brief This enum defines motion sensor ID.
 */
typedef enum
{
    ICM40608 = 0x0001,
    ICMxxxxx = 0x0002
} tpdMotionSensorId_t;

/**
 * @brief This function sets the signal sample rate and range.
 * @param[in] source      signal source (ALG_SOURCE_MOTION or ALG_SOURCE_PRESS).
 * @param[in] sample_rate sample rate of acceleration signal (must > 100Hz).
 * @param[in] sensor_id   sensor id of the signal (ICM40608 or ICMxxxxx).
 * @return    LOG_SUCESS, LOG_SENSOR_ERROR or LOG_SAMPLE_RATE_ERROR.
 */
tpdLog_t tpdSetSampleRate(tpdSigSource_t source, int16_t sample_rate, tpdMotionSensorId_t sensor_id);

/**
 * @brief This function the user information to the library.
 * @param[in] areas          the area sequence set from user (or App).
 * @param[in] area_durations the duration of each area.
 * @param[in] length         the length of areas and area_durations, should <= 6.
 * @return    LOG_SUCESS or LOG_AREA_ERROR.
 */
tpdLog_t tpdSetUserInfo(tpdArea_t *areas, int32_t *area_durations, int32_t length);

/**
 * @brief This function sends the input signal into algorithm library.
 * @param[in] points to a tpd_input_t instance structure
 * @return    none
 */
void tpdAlgProcess(tpdSignal_t *input);

/**
 * @brief This function is to get the version of each algorithm.
 * return 4-byte version code, e.g. v.0.1.0.0. (0x00010000 in hex format).
 */
uint32_t tpdGetAlgVersion(void);

/**
 * @brief This function is to get the algorithm capability.
 * return 4-byte version code, e.g. v.0.0.6.12. (0x0000060C in hex format). 
 */
uint32_t tpdGetAlgCapability(void);

/**
 * @brief Register customized output callback func (use yours).
 */
typedef void (*tpdGetPositionCallback_t)(tpdPosition_t output);
void tpdGetPositionRegister(tpdGetPositionCallback_t func);
void tpdWritePosition(tpdPosition_t position);  /**< Ignore this function. */

/**
 * @brief Register customized memory allocation func.
 */
typedef void* (*tpdMallocCallback_t)(uint32_t size);
void tpdMallocRegister(tpdMallocCallback_t func);

/**
 * @brief Register customized memory free func.
 */
typedef void (*tpdFreeCallback_t)(void* ptr);
void tpdFreeRegister(tpdFreeCallback_t func);

/**
 * @brief Register customized memory log func.
 */
enum {
	LN_LOG_DEBUG   = 0,
	LN_LOG_INFO    = 1,
	LN_LOG_WARNING = 2,
	LN_LOG_ERROR   = 3,
};
typedef int (*tpdLogCallback_t)(int log_level, const char *pcFmt, ...);
void tpdLogRegister(tpdLogCallback_t func);

#endif  /**< __TOOTHBRUSH_POSITION_H__. */

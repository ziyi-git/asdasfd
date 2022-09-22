#include <stdint.h>

typedef float  float32_t;
// typedef double float64_t;

typedef struct
{
    float32_t roll;
    float32_t pitch;
    float32_t yaw;
} lnAttitudeAngle_t;


/**
 * @brief This function initialize all parameters that used to compute angles.
 * param[in]  motion_data 3-axis data of [acc_x, acc_y, acc_z]
 */
void lnAttitudeAngleInit(float32_t motion_data[3]);

void lnAttitudeAngleUninit(void);

/**
 * @brief This function computes angles.
 * @param[in]  motion_data 6-axis data of [acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z].
 * @param[out] angles.
 */
void lnAttitudeAngle(float32_t motion_data[6], float32_t time_stamp, lnAttitudeAngle_t* angles);
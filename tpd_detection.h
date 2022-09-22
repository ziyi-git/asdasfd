#ifndef __TPD_DETECTION_H__
#define __TPD_DETECTION_H__

#pragma once
#include "toothbrush_position_detection.h"
#include "ln_math_types.h"

extern tpdGetPositionCallback_t tpdNotify;

tpdPosition_t* tpdTransferPosition(void);
void tpdSetDataInterval(int16_t sample_rate);
void tpdSetAreaLink(tpdArea_t *areas, int32_t *area_durations, int32_t length);
tpdLog_t tpdSetSurfaceDuration(void);
void tpdDetectInit(void);
void tpdDetectUninit(void);
void tpdDetect(float32_t motion_data[6], int32_t time_stamp);


#endif  /**< __TPD_DETECTION_H__. */

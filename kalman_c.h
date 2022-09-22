#pragma once
#define _USE_MATH_DEFINES
#ifndef __KALMAN_C__
#define __KALMAN_C__
#include <math.h>
#define getRollAcc(ay,az) (-atan2(ay,az)*180/M_PI)
// #define getPitchAcc(ay) (asin(ay)*180/M_PI)
#define getPitchAcc(ax,az) (atan2(ax,az)*180/M_PI)


typedef struct KalmanState
{

	float* qu;
	Kmat*  pk;
} Ks;

#endif
#ifndef __LN_ARRAY_H__
#define __LN_ARRAY_H__

#include "ln_math_types.h"
#include <stdbool.h>

void lnArrSort(float32_t* data, int32_t data_length);
float32_t lnArrMax(const float32_t* data, uint16_t data_length);
float32_t lnArrMean(const float32_t* data, uint16_t data_length);
float32_t lnArrVariance(const float32_t* data, uint16_t data_length, uint16_t ddof);
float32_t lnArrCovariance(const float32_t* x, const float32_t* y, int32_t length, float32_t x_mean, float32_t y_mean);
float32_t lnArrRootMeanSquare(float32_t* data, int32_t data_length);
void lnArrRoll(float32_t* data, int16_t data_length, int16_t shift);
float32_t lnArrMovingAvg(float32_t ndata, float32_t *sum, float32_t *reg, uint8_t *pos, uint8_t len);
float32_t lnArrQuantile(const float32_t* data, uint32_t data_length, float32_t q);
void lnArrAggregateLinearTrend(const float32_t* data, int32_t data_length, int32_t chunk_length,
uint8_t aggregate_method, float32_t* intercept, float32_t* sterrest, float32_t* slope);
float32_t lnArrAutoCorrelation(const float32_t* data, int32_t data_length, uint8_t lag);
float32_t lnArrChangeQuantile(float32_t* data, int32_t data_length, float32_t qh, float32_t ql, bool is_abs,
uint8_t method);

#endif  /**< __LN_ARRAY_H__. */

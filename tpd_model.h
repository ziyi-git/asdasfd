#ifndef __TPD_MODEL_H__
#define __TPD_MODEL_H__

typedef union
{
  int missing;
  // double fvalue;

  float fvalue;
  int qvalue;
} tpdFeat_t;

float tpdPredictArea(tpdFeat_t* data);
float tpdPredictSurface(tpdFeat_t* data);

#endif  /**< __TPD_MODEL_H__. */

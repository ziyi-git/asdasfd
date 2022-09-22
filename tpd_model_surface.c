#include <math.h>
#include <stdlib.h>

#include "tpd_model.h"

static const int th_len[] = {
  98, 66, 79, 37, 40, 70, 75, 48, 
};
static const int th_begin[] = {
  0, 98, 164, 243, 280, 320, 390, 465, 
};

static const float threshold[] = {
  -0.91013634f, -0.89393437f, -0.88984525f, -0.87959969f, -0.85427773f, -0.84438998f, 
  -0.8167429f, -0.8036378f, -0.80302048f, -0.80094826f, -0.791493f, -0.78145325f, 
  -0.76000172f, -0.74679828f, -0.73149383f, -0.71661484f, -0.69572496f, -0.69021416f, 
  -0.68974531f, -0.68187916f, -0.6351018f, -0.62603766f, -0.62499434f, -0.62150812f, 
  -0.56253433f, -0.53861701f, -0.5065881f, -0.48627207f, -0.48537272f, -0.44409728f, 
  -0.43333793f, -0.40371656f, -0.38324904f, -0.36875939f, -0.30693603f, 
  -0.063928783f, -0.049912941f, -0.049376257f, 0.013408868f, 0.050855167f, 
  0.065484986f, 0.22624293f, 0.24145813f, 0.25642902f, 0.33439702f, 0.37270635f, 
  0.43822128f, 0.43840277f, 0.51319581f, 0.55537152f, 0.57945865f, 0.60438108f, 
  0.69412625f, 0.72413516f, 0.76614237f, 0.7691896f, 0.78201669f, 0.80822295f, 
  0.81582797f, 0.82224154f, 0.83661652f, 0.8367641f, 0.83725154f, 0.84788626f, 
  0.84789824f, 0.84906912f, 0.85056734f, 0.85746092f, 0.85925388f, 0.86025631f, 
  0.86557543f, 0.87010837f, 0.87077928f, 0.87266099f, 0.87351608f, 0.87603641f, 
  0.8806172f, 0.88234812f, 0.88309562f, 0.89150774f, 0.89692366f, 0.9013797f, 
  0.91510862f, 0.91676879f, 0.91993231f, 0.94236076f, 0.94240046f, 0.94455808f, 
  0.94541788f, 0.94642949f, 0.95069695f, 0.97609293f, 0.98822111f, 0.99035901f, 
  0.99043536f, 1.0013264f, 1.0497122f, 1.0545065f, -0.33145818f, -0.32220429f, 
  -0.3014943f, -0.24588057f, -0.24003357f, -0.23463753f, -0.23227946f, -0.22078152f, 
  -0.21339992f, -0.20014364f, -0.19414175f, -0.1686991f, -0.16025162f, -0.15928885f, 
  -0.15292183f, -0.14966629f, -0.14209816f, -0.1413862f, -0.14130858f, -0.080186024f, 
  -0.017620893f, -0.0079695694f, -0.0070281411f, -0.002646951f, 0.035640992f, 
  0.036337972f, 0.059329517f, 0.059441298f, 0.065784596f, 0.070886187f, 0.083514564f, 
  0.095990211f, 0.1113591f, 0.123465f, 0.12874882f, 0.1362946f, 0.13942645f, 
  0.16984347f, 0.18195692f, 0.18256229f, 0.19300388f, 0.19305211f, 0.23041752f, 
  0.23633583f, 0.25699103f, 0.25942016f, 0.27397504f, 0.31300351f, 0.32231915f, 
  0.33293015f, 0.35482931f, 0.43187872f, 0.43577105f, 0.44448471f, 0.44464043f, 
  0.44473404f, 0.45270705f, 0.45414662f, 0.45551646f, 0.46294326f, 0.46614644f, 
  0.47188693f, 0.4756304f, 0.50350809f, 0.50356817f, 0.5046469f, 0.014821662f, 
  0.017981224f, 0.023291012f, 0.024709508f, 0.027656078f, 0.028732136f, 0.032067847f, 
  0.033897892f, 0.034039386f, 0.035135359f, 0.043974556f, 0.043976001f, 0.045097984f, 
  0.048153371f, 0.053897209f, 0.054336078f, 0.058834773f, 0.059012689f, 0.059891537f, 
  0.064802393f, 0.067128673f, 0.07173866f, 0.073234782f, 0.073313698f, 0.07798034f, 
  0.09019731f, 0.090832606f, 0.091301307f, 0.092361555f, 0.093177721f, 0.093821317f, 
  0.096691079f, 0.11559273f, 0.14320183f, 0.1781919f, 0.18027547f, 0.18717667f, 
  0.21991791f, 0.23522049f, 0.24543358f, 0.24816349f, 0.25253874f, 0.25410634f, 
  0.28541744f, 0.29028279f, 0.29040265f, 0.29109216f, 0.29210871f, 0.29698992f, 
  0.30297962f, 0.31049407f, 0.31111193f, 0.33292717f, 0.33736977f, 0.3424238f, 
  0.35025495f, 0.35465229f, 0.35825723f, 0.35979754f, 0.35985637f, 0.36013657f, 
  0.36128086f, 0.36308187f, 0.36699542f, 0.37482584f, 0.37485346f, 0.37950248f, 
  0.37975863f, 0.38004988f, 0.38011748f, 0.39104649f, 0.39385736f, 0.40724856f, 
  0.41265863f, 0.46942979f, 0.52353573f, 0.52921796f, 0.54604387f, 0.55169594f, 
  -0.33122864f, -0.17772363f, -0.1421686f, -0.12272961f, -0.10126682f, -0.099330075f, 
  -0.073208183f, -0.064430065f, -0.049215127f, -0.028377928f, -0.027977385f, 
  -0.011083052f, 0.0087004472f, 0.027749004f, 0.029466435f, 0.032594018f, 
  0.032720562f, 0.042550039f, 0.043525063f, 0.06498237f, 0.069233581f, 0.078432351f, 
  0.082996555f, 0.084646657f, 0.091864631f, 0.10418543f, 0.13173442f, 0.14150935f, 
  0.14276412f, 0.15050869f, 0.16209483f, 0.18235677f, 0.18818468f, 0.19305681f, 
  0.20775902f, 0.21178865f, 0.22048701f, 0.0099436492f, 0.021509208f, 0.036447369f, 
  0.03832186f, 0.069275811f, 0.11931758f, 0.16871607f, 0.17652872f, 0.18147863f, 
  0.21398747f, 0.31407499f, 0.34771961f, 0.3577739f, 0.38790441f, 0.3881487f, 
  0.39328426f, 0.39743018f, 0.4026866f, 0.42986494f, 0.43229222f, 0.45626879f, 
  0.46776813f, 0.48458672f, 0.48796266f, 0.49223131f, 0.51101089f, 0.56333959f, 
  0.5661726f, 0.61029333f, 0.61233282f, 0.6143136f, 0.62656748f, 0.63572276f, 
  0.65670085f, 0.67077905f, 0.67457759f, 0.71811676f, 0.85731828f, 0.88475037f, 
  0.94608867f, -0.60196912f, -0.59930736f, -0.55838263f, -0.55826223f, -0.54528153f, 
  -0.54432976f, -0.54365003f, -0.52995247f, -0.51416206f, -0.51283455f, -0.4975819f, 
  -0.48772782f, -0.4839848f, -0.48351088f, -0.4833746f, -0.4507387f, -0.44135517f, 
  -0.42095673f, -0.3944639f, -0.39335606f, -0.38925818f, -0.38667524f, -0.37798458f, 
  -0.37214673f, -0.35197681f, -0.3264479f, -0.30090845f, -0.29898775f, -0.29861432f, 
  -0.27039722f, -0.26190546f, -0.25754386f, -0.18264051f, -0.17136206f, -0.16937938f, 
  -0.12031919f, -0.11913359f, -0.085041761f, -0.075993538f, -0.075565137f, 
  -0.066286981f, -0.064860627f, -0.037729502f, -0.025545394f, -0.024375021f, 
  -0.013885539f, -0.010177309f, 0.002154354f, 0.031244643f, 0.10909928f, 0.11383993f, 
  0.11581217f, 0.13725898f, 0.15270357f, 0.16506444f, 0.17447144f, 0.18186545f, 
  0.24245644f, 0.24964485f, 0.29065052f, 0.31707719f, 0.33771461f, 0.34045959f, 
  0.37276107f, 0.38002616f, 0.38266504f, 0.38378519f, 0.42003709f, 0.42327267f, 
  0.42332572f, -16275.475f, -8967.3574f, -7617.6763f, -5546.1709f, -4847.1953f, 
  -4168.5098f, -3741.0205f, -3727.7556f, -2489.8481f, -1993.6868f, -1987.1465f, 
  -1531.7429f, -1482.2594f, -1257.4902f, -1009.6519f, -981.3844f, -844.46704f, 
  -737.05322f, -504.3114f, -461.37463f, -396.92523f, -374.03027f, -333.53351f, 
  -289.1684f, -289.14999f, -243.99384f, -191.64233f, -164.83592f, -156.89655f, 
  -118.7885f, -116.79521f, -114.26723f, -114.17276f, -93.630447f, -84.098648f, 
  -81.077377f, -79.473175f, -74.277176f, -66.724518f, -66.257912f, -65.679428f, 
  -59.989658f, -58.859917f, -55.857475f, -54.234985f, -48.504517f, -43.321541f, 
  -41.245831f, -40.456699f, -38.017246f, -37.629799f, -27.663555f, -27.10257f, 
  -26.2202f, -25.160135f, -23.52593f, -22.240665f, -20.615318f, -19.41662f, 
  -18.83773f, -18.089413f, -14.120378f, -10.040028f, -9.8230791f, -8.9362316f, 
  -8.2962112f, -6.6246858f, -6.1257153f, -3.7375567f, -3.6251829f, -0.44873554f, 
  -0.064149439f, 1.6867778f, 2.9614997f, 103.73843f, 4.0591345e-08f, 1.528185e-07f, 
  2.1012146e-07f, 6.1258515e-07f, 6.5585391e-07f, 1.2797464e-06f, 1.4440523e-06f, 
  2.3889668e-06f, 2.6989599e-06f, 2.8739382e-06f, 2.9373341e-06f, 3.0302867e-06f, 
  3.0510851e-06f, 3.0551159e-06f, 3.0890978e-06f, 3.413463e-06f, 4.1051608e-06f, 
  4.1580838e-06f, 5.8082528e-06f, 6.5168597e-06f, 9.5154574e-06f, 9.5652267e-06f, 
  1.0322616e-05f, 1.2719769e-05f, 1.3136332e-05f, 1.5841111e-05f, 3.0845506e-05f, 
  3.2234973e-05f, 3.7336162e-05f, 3.7336638e-05f, 4.8805217e-05f, 4.9620801e-05f, 
  5.0106035e-05f, 5.2750202e-05f, 7.1923801e-05f, 7.2366704e-05f, 7.3398245e-05f, 
  7.3833507e-05f, 7.6655735e-05f, 8.1430495e-05f, 8.7962937e-05f, 8.8367553e-05f, 
  0.00011493989f, 0.00011695937f, 0.0001277227f, 0.00013756313f, 0.00016384634f, 
  0.00022584162f, 
};


/*
 * \brief function to convert a feature value into bin index.
 * \param val feature value, in floating-point
 * \param fid feature identifier
 * \return bin index corresponding to given feature value
 */
static inline int quantize(float val, unsigned fid) {
  const size_t offset = th_begin[fid];
  const float* array = &threshold[offset];
  int len = th_len[fid];
  int low = 0;
  int high = len;
  int mid;
  float mval;
  // It is possible th_begin[i] == [total_num_threshold]. This means that
  // all features i, (i+1), ... are not used for any of the splits in the model.
  // So in this case, just return something
  if (offset == 513 || val < array[0]) {
    return -10;
  }
  while (low + 1 < high) {
    mid = (low + high) / 2;
    mval = array[mid];
    if (val == mval) {
      return mid * 2;
    } else if (val < mval) {
      high = mid;
    } else {
      low = mid;
    }
  }
  if (array[low] == val) {
    return low * 2;
  } else if (high == len) {
    return len * 2;
  } else {
    return low * 2 + 1;
  }
}


const unsigned char is_categorical[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 
};

float tpdPredictSurface(tpdFeat_t* data) {

  for (int i = 0; i < 8; ++i) {
    if (data[i].missing != -1 && !is_categorical[i]) {
      data[i].qvalue = quantize(data[i].fvalue, i);
    }
  }
  float sum = (float)0;
//   unsigned int tmp;
//   int nid, cond, fid;  /* used for folded subtrees */
  if (!(data[1].missing != -1) || (data[1].qvalue < 54)) {
    if (!(data[2].missing != -1) || (data[2].qvalue < 16)) {
      if (!(data[1].missing != -1) || (data[1].qvalue < 20)) {
        if (!(data[7].missing != -1) || (data[7].qvalue < 82)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 28)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 14)) {
              sum += 0.58087652922f;
            } else {
              sum += 0.15000000596f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 50)) {
              sum += 0.077419355512f;
            } else {
              sum += 0.50649350882f;
            }
          }
        } else {
          if (!(data[5].missing != -1) || (data[5].qvalue < 10)) {
            sum += 0.36000001431f;
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 18)) {
              sum += -0.48000001907f;
            } else {
              sum += -.0f;
            }
          }
        }
      } else {
        if (!(data[0].missing != -1) || (data[0].qvalue < 28)) {
          sum += -0.59558826685f;
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 142)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 98)) {
              sum += 0.1858407259f;
            } else {
              sum += 0.53684210777f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 192)) {
              sum += -0.40592256188f;
            } else {
              sum += 0.11186441034f;
            }
          }
        }
      }
    } else {
      if (!(data[1].missing != -1) || (data[1].qvalue < 32)) {
        if (!(data[0].missing != -1) || (data[0].qvalue < 154)) {
          if (!(data[1].missing != -1) || (data[1].qvalue < 8)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 12)) {
              sum += 0.208695665f;
            } else {
              sum += 0.56562578678f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 48)) {
              sum += 0.53695654869f;
            } else {
              sum += 0.28045326471f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 98)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 58)) {
              sum += 0.49263158441f;
            } else {
              sum += 0.28846153617f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 14)) {
              sum += 0.41359224916f;
            } else {
              sum += -0.06453089416f;
            }
          }
        }
      } else {
        if (!(data[0].missing != -1) || (data[0].qvalue < 50)) {
          if (!(data[0].missing != -1) || (data[0].qvalue < 40)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 90)) {
              sum += -0.56000000238f;
            } else {
              sum += -0.066666670144f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 86)) {
              sum += 0.026086958125f;
            } else {
              sum += -0.47368425131f;
            }
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 104)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 130)) {
              sum += 0.37471953034f;
            } else {
              sum += -0.090058475733f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 74)) {
              sum += 0.2265470922f;
            } else {
              sum += -0.14955490828f;
            }
          }
        }
      }
    }
  } else {
    if (!(data[5].missing != -1) || (data[5].qvalue < 104)) {
      if (!(data[0].missing != -1) || (data[0].qvalue < 24)) {
        if (!(data[6].missing != -1) || (data[6].qvalue < 82)) {
          if (!(data[4].missing != -1) || (data[4].qvalue < 40)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 8)) {
              sum += -0.051063831896f;
            } else {
              sum += -0.39545458555f;
            }
          } else {
            if (!(data[3].missing != -1) || (data[3].qvalue < 66)) {
              sum += -0.48730966449f;
            } else {
              sum += -.0f;
            }
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 70)) {
            if (!(data[3].missing != -1) || (data[3].qvalue < 50)) {
              sum += -0.58449304104f;
            } else {
              sum += -.0f;
            }
          } else {
            sum += -0.054545458406f;
          }
        }
      } else {
        if (!(data[2].missing != -1) || (data[2].qvalue < 114)) {
          if (!(data[0].missing != -1) || (data[0].qvalue < 138)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 64)) {
              sum += 0.20765833557f;
            } else {
              sum += -0.1531457901f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 60)) {
              sum += 0.24705883861f;
            } else {
              sum += -0.53430658579f;
            }
          }
        } else {
          if (!(data[3].missing != -1) || (data[3].qvalue < 70)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 72)) {
              sum += -0.50851488113f;
            } else {
              sum += -0.28556150198f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 96)) {
              sum += 0.36315792799f;
            } else {
              sum += -0.23076924682f;
            }
          }
        }
      }
    } else {
      if (!(data[1].missing != -1) || (data[1].qvalue < 114)) {
        if (!(data[0].missing != -1) || (data[0].qvalue < 16)) {
          if (!(data[1].missing != -1) || (data[1].qvalue < 84)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 110)) {
              sum += -0.56288659573f;
            } else {
              sum += 0.089361704886f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 54)) {
              sum += -0.39840000868f;
            } else {
              sum += -0.57696336508f;
            }
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 32)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 120)) {
              sum += -0.30000001192f;
            } else {
              sum += 0.41538465023f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 130)) {
              sum += -0.45970067382f;
            } else {
              sum += -0.17528089881f;
            }
          }
        }
      } else {
        if (!(data[0].missing != -1) || (data[0].qvalue < 64)) {
          if (!(data[0].missing != -1) || (data[0].qvalue < 6)) {
            sum += -0.15000000596f;
          } else {
            sum += -0.56903713942f;
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 34)) {
            sum += -.0f;
          } else {
            sum += -0.30000001192f;
          }
        }
      }
    }
  }
  if (!(data[1].missing != -1) || (data[1].qvalue < 52)) {
    if (!(data[2].missing != -1) || (data[2].qvalue < 12)) {
      if (!(data[1].missing != -1) || (data[1].qvalue < 22)) {
        if (!(data[0].missing != -1) || (data[0].qvalue < 162)) {
          if (!(data[0].missing != -1) || (data[0].qvalue < 90)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 88)) {
              sum += 0.35477519035f;
            } else {
              sum += -0.36290282011f;
            }
          } else {
            sum += 0.46366658807f;
          }
        } else {
          if (!(data[4].missing != -1) || (data[4].qvalue < 50)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 16)) {
              sum += 0.39956316352f;
            } else {
              sum += -0.28342345357f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 20)) {
              sum += 0.44254601002f;
            } else {
              sum += 0.18336123228f;
            }
          }
        }
      } else {
        if (!(data[0].missing != -1) || (data[0].qvalue < 36)) {
          if (!(data[3].missing != -1) || (data[3].qvalue < 44)) {
            sum += -0.46357122064f;
          } else {
            sum += -0.13414996862f;
          }
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 168)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 66)) {
              sum += 0.30422195792f;
            } else {
              sum += -0.23711843789f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 192)) {
              sum += -0.34486484528f;
            } else {
              sum += 0.09380479902f;
            }
          }
        }
      }
    } else {
      if (!(data[1].missing != -1) || (data[1].qvalue < 32)) {
        if (!(data[0].missing != -1) || (data[0].qvalue < 148)) {
          if (!(data[1].missing != -1) || (data[1].qvalue < 6)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 12)) {
              sum += 0.13552963734f;
            } else {
              sum += 0.43789955974f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 44)) {
              sum += 0.41702952981f;
            } else {
              sum += 0.21719700098f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 94)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 178)) {
              sum += 0.3666062057f;
            } else {
              sum += 0.18383419514f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 8)) {
              sum += 0.50001972914f;
            } else {
              sum += -0.0312246494f;
            }
          }
        }
      } else {
        if (!(data[0].missing != -1) || (data[0].qvalue < 172)) {
          if (!(data[0].missing != -1) || (data[0].qvalue < 54)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 46)) {
              sum += -0.42310708761f;
            } else {
              sum += -0.15015099943f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 100)) {
              sum += 0.27409499884f;
            } else {
              sum += 0.069069251418f;
            }
          }
        } else {
          if (!(data[4].missing != -1) || (data[4].qvalue < 46)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 58)) {
              sum += -0.51256340742f;
            } else {
              sum += 0.21524176002f;
            }
          } else {
            if (!(data[3].missing != -1) || (data[3].qvalue < 12)) {
              sum += 0.29737579823f;
            } else {
              sum += -0.031419537961f;
            }
          }
        }
      }
    }
  } else {
    if (!(data[3].missing != -1) || (data[3].qvalue < 72)) {
      if (!(data[0].missing != -1) || (data[0].qvalue < 42)) {
        if (!(data[6].missing != -1) || (data[6].qvalue < 104)) {
          if (!(data[4].missing != -1) || (data[4].qvalue < 44)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 112)) {
              sum += -0.21845196187f;
            } else {
              sum += -0.42108485103f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 56)) {
              sum += -0.34056717157f;
            } else {
              sum += -0.41091144085f;
            }
          }
        } else {
          if (!(data[4].missing != -1) || (data[4].qvalue < 0)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 74)) {
              sum += 0.42829713225f;
            } else {
              sum += -0.25355431437f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 56)) {
              sum += -0.19992549717f;
            } else {
              sum += -0.44090485573f;
            }
          }
        }
      } else {
        if (!(data[5].missing != -1) || (data[5].qvalue < 108)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 124)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 74)) {
              sum += 0.0029436491895f;
            } else {
              sum += -0.32836565375f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 70)) {
              sum += -0.40022203326f;
            } else {
              sum += -0.22130395472f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 122)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 90)) {
              sum += -0.30001917481f;
            } else {
              sum += -0.40819552541f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 122)) {
              sum += -0.32174730301f;
            } else {
              sum += -0.017145447433f;
            }
          }
        }
      }
    } else {
      if (!(data[5].missing != -1) || (data[5].qvalue < 98)) {
        if (!(data[2].missing != -1) || (data[2].qvalue < 144)) {
          if (!(data[7].missing != -1) || (data[7].qvalue < 32)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 70)) {
              sum += 0.43527945876f;
            } else {
              sum += 0.013157797046f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 96)) {
              sum += 0.45025020838f;
            } else {
              sum += 0.1784863323f;
            }
          }
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 84)) {
            if (!(data[4].missing != -1) || (data[4].qvalue < 76)) {
              sum += -0.16730545461f;
            } else {
              sum += 0.22443029284f;
            }
          } else {
            sum += 0.41776141524f;
          }
        }
      } else {
        if (!(data[0].missing != -1) || (data[0].qvalue < 68)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 74)) {
            sum += -0.21802258492f;
          } else {
            sum += 0.49714666605f;
          }
        } else {
          sum += -0.3488933146f;
        }
      }
    }
  }
  if (!(data[1].missing != -1) || (data[1].qvalue < 50)) {
    if (!(data[1].missing != -1) || (data[1].qvalue < 30)) {
      if (!(data[0].missing != -1) || (data[0].qvalue < 158)) {
        if (!(data[1].missing != -1) || (data[1].qvalue < 8)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 34)) {
            if (!(data[3].missing != -1) || (data[3].qvalue < 68)) {
              sum += 0.39098778367f;
            } else {
              sum += -0.24008636177f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 26)) {
              sum += 0.25297236443f;
            } else {
              sum += 0.3738797307f;
            }
          }
        } else {
          if (!(data[7].missing != -1) || (data[7].qvalue < 46)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 86)) {
              sum += 0.39251077175f;
            } else {
              sum += 0.27861121297f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 108)) {
              sum += 0.2606998682f;
            } else {
              sum += -0.1556121558f;
            }
          }
        }
      } else {
        if (!(data[6].missing != -1) || (data[6].qvalue < 96)) {
          if (!(data[7].missing != -1) || (data[7].qvalue < 56)) {
            if (!(data[3].missing != -1) || (data[3].qvalue < 58)) {
              sum += 0.33122894168f;
            } else {
              sum += -0.020913349465f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 18)) {
              sum += 0.33249330521f;
            } else {
              sum += -0.012627239339f;
            }
          }
        } else {
          if (!(data[7].missing != -1) || (data[7].qvalue < 18)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 44)) {
              sum += -0.34481996298f;
            } else {
              sum += 0.3819154799f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 12)) {
              sum += 0.38524642587f;
            } else {
              sum += -0.18193972111f;
            }
          }
        }
      }
    } else {
      if (!(data[2].missing != -1) || (data[2].qvalue < 20)) {
        if (!(data[0].missing != -1) || (data[0].qvalue < 36)) {
          sum += -0.40184879303f;
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 144)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 46)) {
              sum += -0.090318135917f;
            } else {
              sum += 0.28890672326f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 66)) {
              sum += -0.31873720884f;
            } else {
              sum += -0.092933692038f;
            }
          }
        }
      } else {
        if (!(data[0].missing != -1) || (data[0].qvalue < 160)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 96)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 72)) {
              sum += 0.24282862246f;
            } else {
              sum += 0.022836145014f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 150)) {
              sum += 0.050395507365f;
            } else {
              sum += 0.48667746782f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 80)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 20)) {
              sum += -0.097076274455f;
            } else {
              sum += 0.28328517079f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 46)) {
              sum += -0.42584758997f;
            } else {
              sum += -0.028926514089f;
            }
          }
        }
      }
    }
  } else {
    if (!(data[0].missing != -1) || (data[0].qvalue < 44)) {
      if (!(data[6].missing != -1) || (data[6].qvalue < 90)) {
        if (!(data[1].missing != -1) || (data[1].qvalue < 112)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 126)) {
            if (!(data[4].missing != -1) || (data[4].qvalue < 16)) {
              sum += 0.012994780205f;
            } else {
              sum += -0.25140103698f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 26)) {
              sum += 0.38886618614f;
            } else {
              sum += -0.30928868055f;
            }
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 46)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 128)) {
              sum += -0.081722930074f;
            } else {
              sum += -0.39563646913f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 10)) {
              sum += -0.1506011039f;
            } else {
              sum += -0.39789429307f;
            }
          }
        }
      } else {
        if (!(data[4].missing != -1) || (data[4].qvalue < 0)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 48)) {
            sum += 0.34740972519f;
          } else {
            sum += -0.28503006697f;
          }
        } else {
          if (!(data[4].missing != -1) || (data[4].qvalue < 24)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 36)) {
              sum += -0.07115868479f;
            } else {
              sum += -0.36184537411f;
            }
          } else {
            if (!(data[3].missing != -1) || (data[3].qvalue < 56)) {
              sum += -0.37913742661f;
            } else {
              sum += 0.14676578343f;
            }
          }
        }
      }
    } else {
      if (!(data[5].missing != -1) || (data[5].qvalue < 106)) {
        if (!(data[2].missing != -1) || (data[2].qvalue < 118)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 36)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 62)) {
              sum += 0.31462907791f;
            } else {
              sum += -0.40799623728f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 68)) {
              sum += 0.18863156438f;
            } else {
              sum += -0.056679341942f;
            }
          }
        } else {
          if (!(data[3].missing != -1) || (data[3].qvalue < 46)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 76)) {
              sum += -0.32759973407f;
            } else {
              sum += -0.13862901926f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 88)) {
              sum += 0.26222500205f;
            } else {
              sum += -0.10317962617f;
            }
          }
        }
      } else {
        if (!(data[7].missing != -1) || (data[7].qvalue < 16)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 34)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 8)) {
              sum += -0.30222660303f;
            } else {
              sum += 0.33685985208f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 18)) {
              sum += 0.10582039505f;
            } else {
              sum += -0.23983086646f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 148)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 6)) {
              sum += 0.22278997302f;
            } else {
              sum += -0.32014501095f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 114)) {
              sum += 0.49860954285f;
            } else {
              sum += -0.088012568653f;
            }
          }
        }
      }
    }
  }
  if (!(data[1].missing != -1) || (data[1].qvalue < 48)) {
    if (!(data[1].missing != -1) || (data[1].qvalue < 28)) {
      if (!(data[0].missing != -1) || (data[0].qvalue < 150)) {
        if (!(data[5].missing != -1) || (data[5].qvalue < 38)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 14)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 116)) {
              sum += 0.13492847979f;
            } else {
              sum += -0.35088834167f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 32)) {
              sum += 0.21173854172f;
            } else {
              sum += 0.35716295242f;
            }
          }
        } else {
          if (!(data[7].missing != -1) || (data[7].qvalue < 60)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 26)) {
              sum += 0.33994975686f;
            } else {
              sum += 0.20806372166f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 104)) {
              sum += 0.1313508153f;
            } else {
              sum += -0.38294583559f;
            }
          }
        }
      } else {
        if (!(data[5].missing != -1) || (data[5].qvalue < 6)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 72)) {
            sum += 0.35908797383f;
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 164)) {
              sum += 0.24284990132f;
            } else {
              sum += -0.090844780207f;
            }
          }
        } else {
          if (!(data[4].missing != -1) || (data[4].qvalue < 60)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 42)) {
              sum += -0.15534074605f;
            } else {
              sum += 0.090575039387f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 16)) {
              sum += 0.25369307399f;
            } else {
              sum += 0.054445203394f;
            }
          }
        }
      }
    } else {
      if (!(data[2].missing != -1) || (data[2].qvalue < 26)) {
        if (!(data[0].missing != -1) || (data[0].qvalue < 48)) {
          if (!(data[0].missing != -1) || (data[0].qvalue < 36)) {
            sum += -0.36799019575f;
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 84)) {
              sum += -0.40200462937f;
            } else {
              sum += 0.2247941643f;
            }
          }
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 152)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 136)) {
              sum += 0.06933221966f;
            } else {
              sum += 0.36668366194f;
            }
          } else {
            if (!(data[3].missing != -1) || (data[3].qvalue < 18)) {
              sum += 0.14161604643f;
            } else {
              sum += -0.21018743515f;
            }
          }
        }
      } else {
        if (!(data[2].missing != -1) || (data[2].qvalue < 100)) {
          if (!(data[7].missing != -1) || (data[7].qvalue < 28)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 44)) {
              sum += 0.34327828884f;
            } else {
              sum += -0.0027667144313f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 112)) {
              sum += 0.141352579f;
            } else {
              sum += -0.024037366733f;
            }
          }
        } else {
          if (!(data[4].missing != -1) || (data[4].qvalue < 14)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 122)) {
              sum += -0.56137996912f;
            } else {
              sum += 0.078646808863f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 72)) {
              sum += 0.1759428978f;
            } else {
              sum += -0.03478743881f;
            }
          }
        }
      }
    }
  } else {
    if (!(data[0].missing != -1) || (data[0].qvalue < 42)) {
      if (!(data[6].missing != -1) || (data[6].qvalue < 108)) {
        if (!(data[1].missing != -1) || (data[1].qvalue < 112)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 64)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 106)) {
              sum += -0.26377868652f;
            } else {
              sum += 0.24315358698f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 100)) {
              sum += 0.0022356049158f;
            } else {
              sum += -0.21803154051f;
            }
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 46)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 60)) {
              sum += -0.078293561935f;
            } else {
              sum += -0.34472507238f;
            }
          } else {
            sum += -0.3540121913f;
          }
        }
      } else {
        if (!(data[4].missing != -1) || (data[4].qvalue < 4)) {
          if (!(data[0].missing != -1) || (data[0].qvalue < 18)) {
            sum += -0.2713675499f;
          } else {
            sum += 0.32104885578f;
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 52)) {
            if (!(data[4].missing != -1) || (data[4].qvalue < 8)) {
              sum += 0.087469063699f;
            } else {
              sum += -0.35766136646f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 70)) {
              sum += 0.17536357045f;
            } else {
              sum += -0.31111592054f;
            }
          }
        }
      }
    } else {
      if (!(data[5].missing != -1) || (data[5].qvalue < 108)) {
        if (!(data[2].missing != -1) || (data[2].qvalue < 128)) {
          if (!(data[0].missing != -1) || (data[0].qvalue < 136)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 116)) {
              sum += -0.018560871482f;
            } else {
              sum += 0.21031427383f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 74)) {
              sum += -0.10607735068f;
            } else {
              sum += -0.38365450501f;
            }
          }
        } else {
          if (!(data[3].missing != -1) || (data[3].qvalue < 46)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 74)) {
              sum += -0.30249145627f;
            } else {
              sum += -0.14735579491f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 88)) {
              sum += 0.18603679538f;
            } else {
              sum += -0.094384305179f;
            }
          }
        }
      } else {
        if (!(data[6].missing != -1) || (data[6].qvalue < 110)) {
          if (!(data[1].missing != -1) || (data[1].qvalue < 90)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 52)) {
              sum += 0.37634569407f;
            } else {
              sum += -0.34845209122f;
            }
          } else {
            if (!(data[3].missing != -1) || (data[3].qvalue < 54)) {
              sum += -0.30703938007f;
            } else {
              sum += 0.0075178700499f;
            }
          }
        } else {
          if (!(data[1].missing != -1) || (data[1].qvalue < 94)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 70)) {
              sum += -0.30922108889f;
            } else {
              sum += 0.17525544763f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 56)) {
              sum += -0.1848859936f;
            } else {
              sum += 0.11039166152f;
            }
          }
        }
      }
    }
  }
  if (!(data[1].missing != -1) || (data[1].qvalue < 58)) {
    if (!(data[1].missing != -1) || (data[1].qvalue < 12)) {
      if (!(data[0].missing != -1) || (data[0].qvalue < 158)) {
        if (!(data[5].missing != -1) || (data[5].qvalue < 34)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 32)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 122)) {
              sum += 0.26229500771f;
            } else {
              sum += -0.2703909874f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 140)) {
              sum += 0.34097123146f;
            } else {
              sum += 0.2328338474f;
            }
          }
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 102)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 4)) {
              sum += -0.051322184503f;
            } else {
              sum += 0.3321044147f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 28)) {
              sum += -0.061646856368f;
            } else {
              sum += 0.27710053325f;
            }
          }
        }
      } else {
        if (!(data[6].missing != -1) || (data[6].qvalue < 118)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 6)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 182)) {
              sum += 0.14669439197f;
            } else {
              sum += -0.32705038786f;
            }
          } else {
            if (!(data[3].missing != -1) || (data[3].qvalue < 32)) {
              sum += 0.25205013156f;
            } else {
              sum += 0.071757473052f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 140)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 56)) {
              sum += -0.28277599812f;
            } else {
              sum += 0.35651397705f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 28)) {
              sum += 0.36770430207f;
            } else {
              sum += -0.042951613665f;
            }
          }
        }
      }
    } else {
      if (!(data[2].missing != -1) || (data[2].qvalue < 22)) {
        if (!(data[0].missing != -1) || (data[0].qvalue < 28)) {
          if (!(data[3].missing != -1) || (data[3].qvalue < 42)) {
            sum += -0.34902721643f;
          } else {
            sum += -0.0873843804f;
          }
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 132)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 92)) {
              sum += 0.02906816639f;
            } else {
              sum += 0.31999444962f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 78)) {
              sum += -0.046129442751f;
            } else {
              sum += -0.30819994211f;
            }
          }
        }
      } else {
        if (!(data[7].missing != -1) || (data[7].qvalue < 54)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 90)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 44)) {
              sum += 0.057883877307f;
            } else {
              sum += 0.21078127623f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 10)) {
              sum += -0.25408664346f;
            } else {
              sum += 0.06946156919f;
            }
          }
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 106)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 54)) {
              sum += 0.32345309854f;
            } else {
              sum += 0.014002288692f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 32)) {
              sum += 0.14395263791f;
            } else {
              sum += -0.13284596801f;
            }
          }
        }
      }
    }
  } else {
    if (!(data[3].missing != -1) || (data[3].qvalue < 60)) {
      if (!(data[1].missing != -1) || (data[1].qvalue < 108)) {
        if (!(data[0].missing != -1) || (data[0].qvalue < 12)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 88)) {
            if (!(data[4].missing != -1) || (data[4].qvalue < 20)) {
              sum += 0.12624135613f;
            } else {
              sum += -0.21477197111f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 52)) {
              sum += -0.33698177338f;
            } else {
              sum += -0.23533910513f;
            }
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 126)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 76)) {
              sum += -0.064970560372f;
            } else {
              sum += -0.25179892778f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 48)) {
              sum += -0.14089746773f;
            } else {
              sum += -0.28275215626f;
            }
          }
        }
      } else {
        if (!(data[0].missing != -1) || (data[0].qvalue < 30)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 58)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 134)) {
              sum += -0.15461459756f;
            } else {
              sum += 0.63101667166f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 122)) {
              sum += -0.24643066525f;
            } else {
              sum += -0.34250694513f;
            }
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 98)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 2)) {
              sum += -0.057465340942f;
            } else {
              sum += -0.35183262825f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 116)) {
              sum += -0.27997270226f;
            } else {
              sum += 0.52953660488f;
            }
          }
        }
      }
    } else {
      if (!(data[5].missing != -1) || (data[5].qvalue < 100)) {
        if (!(data[2].missing != -1) || (data[2].qvalue < 134)) {
          if (!(data[4].missing != -1) || (data[4].qvalue < 78)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 62)) {
              sum += -0.10539137572f;
            } else {
              sum += 0.28366687894f;
            }
          } else {
            sum += -0.080555826426f;
          }
        } else {
          if (!(data[5].missing != -1) || (data[5].qvalue < 96)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 38)) {
              sum += -0.15156275034f;
            } else {
              sum += 0.17596270144f;
            }
          } else {
            sum += -0.22823353112f;
          }
        }
      } else {
        if (!(data[7].missing != -1) || (data[7].qvalue < 30)) {
          sum += 0.28765168786f;
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 76)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 40)) {
              sum += -0.079709835351f;
            } else {
              sum += -0.32961541414f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 60)) {
              sum += 0.33367979527f;
            } else {
              sum += -0.16730603576f;
            }
          }
        }
      }
    }
  }
  if (!(data[0].missing != -1) || (data[0].qvalue < 78)) {
    if (!(data[0].missing != -1) || (data[0].qvalue < 58)) {
      if (!(data[4].missing != -1) || (data[4].qvalue < 48)) {
        if (!(data[1].missing != -1) || (data[1].qvalue < 98)) {
          if (!(data[0].missing != -1) || (data[0].qvalue < 22)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 54)) {
              sum += -0.26505997777f;
            } else {
              sum += -0.045772068202f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 106)) {
              sum += 0.12945097685f;
            } else {
              sum += -0.29933556914f;
            }
          }
        } else {
          if (!(data[5].missing != -1) || (data[5].qvalue < 124)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 42)) {
              sum += -0.045528955758f;
            } else {
              sum += -0.29557403922f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 102)) {
              sum += 0.24077375233f;
            } else {
              sum += -0.18231363595f;
            }
          }
        }
      } else {
        if (!(data[1].missing != -1) || (data[1].qvalue < 36)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 82)) {
            sum += 0.33615803719f;
          } else {
            sum += 0.027617303655f;
          }
        } else {
          if (!(data[3].missing != -1) || (data[3].qvalue < 52)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 82)) {
              sum += -0.32498338819f;
            } else {
              sum += -0.20777313411f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 26)) {
              sum += 0.33246135712f;
            } else {
              sum += -0.044151425362f;
            }
          }
        }
      }
    } else {
      if (!(data[2].missing != -1) || (data[2].qvalue < 138)) {
        if (!(data[1].missing != -1) || (data[1].qvalue < 76)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 102)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 68)) {
              sum += -0.0065459348261f;
            } else {
              sum += 0.17265759408f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 110)) {
              sum += 0.64253634214f;
            } else {
              sum += -0.22841618955f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 126)) {
            if (!(data[3].missing != -1) || (data[3].qvalue < 48)) {
              sum += -0.22700069845f;
            } else {
              sum += 0.071309715509f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 66)) {
              sum += 0.41775974631f;
            } else {
              sum += -0.15207344294f;
            }
          }
        }
      } else {
        if (!(data[6].missing != -1) || (data[6].qvalue < 46)) {
          if (!(data[4].missing != -1) || (data[4].qvalue < 34)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 42)) {
              sum += -0.33553236723f;
            } else {
              sum += -0.035292170942f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 62)) {
              sum += 0.08363455534f;
            } else {
              sum += -0.23880042136f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 146)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 156)) {
              sum += -0.25858035684f;
            } else {
              sum += 0.27701804042f;
            }
          } else {
            sum += 0.35969007015f;
          }
        }
      }
    }
  } else {
    if (!(data[0].missing != -1) || (data[0].qvalue < 174)) {
      if (!(data[5].missing != -1) || (data[5].qvalue < 30)) {
        if (!(data[6].missing != -1) || (data[6].qvalue < 30)) {
          if (!(data[3].missing != -1) || (data[3].qvalue < 6)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 0)) {
              sum += 0.083030417562f;
            } else {
              sum += -0.43642407656f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 118)) {
              sum += 0.21575258672f;
            } else {
              sum += -0.044869106263f;
            }
          }
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 134)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 94)) {
              sum += -0.10869180411f;
            } else {
              sum += 0.32800501585f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 0)) {
              sum += 0.10244309902f;
            } else {
              sum += 0.3021979332f;
            }
          }
        }
      } else {
        if (!(data[7].missing != -1) || (data[7].qvalue < 10)) {
          if (!(data[0].missing != -1) || (data[0].qvalue < 86)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 132)) {
              sum += 0.29065591097f;
            } else {
              sum += -0.092445932329f;
            }
          } else {
            if (!(data[3].missing != -1) || (data[3].qvalue < 2)) {
              sum += -0.28506550193f;
            } else {
              sum += 0.31509017944f;
            }
          }
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 114)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 48)) {
              sum += 0.23037590086f;
            } else {
              sum += 0.072470672429f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 78)) {
              sum += 0.14248675108f;
            } else {
              sum += -0.19981244206f;
            }
          }
        }
      }
    } else {
      if (!(data[5].missing != -1) || (data[5].qvalue < 80)) {
        if (!(data[5].missing != -1) || (data[5].qvalue < 14)) {
          if (!(data[4].missing != -1) || (data[4].qvalue < 72)) {
            if (!(data[3].missing != -1) || (data[3].qvalue < 36)) {
              sum += 0.35722893476f;
            } else {
              sum += 0.11933406442f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 184)) {
              sum += -0.1255235523f;
            } else {
              sum += 0.25369277596f;
            }
          }
        } else {
          if (!(data[4].missing != -1) || (data[4].qvalue < 42)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 194)) {
              sum += -0.21039517224f;
            } else {
              sum += 0.087510272861f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 186)) {
              sum += -0.094583965838f;
            } else {
              sum += 0.12105349451f;
            }
          }
        }
      } else {
        if (!(data[3].missing != -1) || (data[3].qvalue < 14)) {
          sum += 0.31629404426f;
        } else {
          if (!(data[1].missing != -1) || (data[1].qvalue < 78)) {
            if (!(data[3].missing != -1) || (data[3].qvalue < 20)) {
              sum += -0.12290130556f;
            } else {
              sum += -0.36303135753f;
            }
          } else {
            sum += 0.17695465684f;
          }
        }
      }
    }
  }
  if (!(data[1].missing != -1) || (data[1].qvalue < 60)) {
    if (!(data[1].missing != -1) || (data[1].qvalue < 10)) {
      if (!(data[0].missing != -1) || (data[0].qvalue < 158)) {
        if (!(data[6].missing != -1) || (data[6].qvalue < 130)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 12)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 8)) {
              sum += 0.099469259381f;
            } else {
              sum += -0.35270884633f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 120)) {
              sum += 0.28278955817f;
            } else {
              sum += 0.17691330612f;
            }
          }
        } else {
          if (!(data[5].missing != -1) || (data[5].qvalue < 40)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 94)) {
              sum += 0.29654914141f;
            } else {
              sum += 0.047959264368f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 36)) {
              sum += -0.27925562859f;
            } else {
              sum += 0.27837643027f;
            }
          }
        }
      } else {
        if (!(data[6].missing != -1) || (data[6].qvalue < 100)) {
          if (!(data[1].missing != -1) || (data[1].qvalue < 4)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 36)) {
              sum += 0.3025983274f;
            } else {
              sum += -0.014969532378f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 52)) {
              sum += -0.10357402265f;
            } else {
              sum += 0.2555963397f;
            }
          }
        } else {
          if (!(data[4].missing != -1) || (data[4].qvalue < 22)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 0)) {
              sum += 0.20741377771f;
            } else {
              sum += -0.29026079178f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 22)) {
              sum += 0.39999490976f;
            } else {
              sum += -0.039864420891f;
            }
          }
        }
      }
    } else {
      if (!(data[0].missing != -1) || (data[0].qvalue < 20)) {
        if (!(data[2].missing != -1) || (data[2].qvalue < 30)) {
          if (!(data[3].missing != -1) || (data[3].qvalue < 38)) {
            sum += -0.32574915886f;
          } else {
            sum += -0.06178015843f;
          }
        } else {
          sum += 0.078717663884f;
        }
      } else {
        if (!(data[0].missing != -1) || (data[0].qvalue < 170)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 132)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 152)) {
              sum += 0.037190567702f;
            } else {
              sum += 0.42490747571f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 52)) {
              sum += 0.19866894186f;
            } else {
              sum += -0.0009160420741f;
            }
          }
        } else {
          if (!(data[5].missing != -1) || (data[5].qvalue < 82)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 186)) {
              sum += -0.12971365452f;
            } else {
              sum += 0.024672865868f;
            }
          } else {
            if (!(data[3].missing != -1) || (data[3].qvalue < 14)) {
              sum += 0.3556573987f;
            } else {
              sum += -0.32716757059f;
            }
          }
        }
      }
    }
  } else {
    if (!(data[1].missing != -1) || (data[1].qvalue < 118)) {
      if (!(data[0].missing != -1) || (data[0].qvalue < 4)) {
        if (!(data[7].missing != -1) || (data[7].qvalue < 86)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 60)) {
            if (!(data[4].missing != -1) || (data[4].qvalue < 6)) {
              sum += 0.068170554936f;
            } else {
              sum += -0.28778082132f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 92)) {
              sum += 0.39493444562f;
            } else {
              sum += -0.2347228229f;
            }
          }
        } else {
          if (!(data[7].missing != -1) || (data[7].qvalue < 88)) {
            sum += 0.25596764684f;
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 54)) {
              sum += -0.22613544762f;
            } else {
              sum += 0.12068527192f;
            }
          }
        }
      } else {
        if (!(data[2].missing != -1) || (data[2].qvalue < 130)) {
          if (!(data[3].missing != -1) || (data[3].qvalue < 64)) {
            if (!(data[4].missing != -1) || (data[4].qvalue < 68)) {
              sum += -0.03727793321f;
            } else {
              sum += -0.14703392982f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 32)) {
              sum += 0.0066503803246f;
            } else {
              sum += 0.1944873035f;
            }
          }
        } else {
          if (!(data[3].missing != -1) || (data[3].qvalue < 40)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 42)) {
              sum += -0.28060784936f;
            } else {
              sum += -0.15129452944f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 38)) {
              sum += 0.15752734244f;
            } else {
              sum += -0.15340407193f;
            }
          }
        }
      }
    } else {
      if (!(data[0].missing != -1) || (data[0].qvalue < 34)) {
        if (!(data[6].missing != -1) || (data[6].qvalue < 58)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 128)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 124)) {
              sum += 0.010276050307f;
            } else {
              sum += -0.29278436303f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 130)) {
              sum += 0.49182474613f;
            } else {
              sum += -0.21734067798f;
            }
          }
        } else {
          if (!(data[3].missing != -1) || (data[3].qvalue < 30)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 32)) {
              sum += -0.31801947951f;
            } else {
              sum += -0.02009120211f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 50)) {
              sum += 0.12849676609f;
            } else {
              sum += -0.21133394539f;
            }
          }
        }
      } else {
        if (!(data[3].missing != -1) || (data[3].qvalue < 62)) {
          sum += -0.33106189966f;
        } else {
          if (!(data[7].missing != -1) || (data[7].qvalue < 50)) {
            sum += 0.14104239643f;
          } else {
            sum += -0.21136114001f;
          }
        }
      }
    }
  }
  if (!(data[1].missing != -1) || (data[1].qvalue < 34)) {
    if (!(data[0].missing != -1) || (data[0].qvalue < 114)) {
      if (!(data[2].missing != -1) || (data[2].qvalue < 68)) {
        if (!(data[2].missing != -1) || (data[2].qvalue < 0)) {
          if (!(data[4].missing != -1) || (data[4].qvalue < 12)) {
            sum += -0.18234594166f;
          } else {
            sum += 0.19464759529f;
          }
        } else {
          if (!(data[1].missing != -1) || (data[1].qvalue < 24)) {
            sum += 0.31359615922f;
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 110)) {
              sum += 0.32998088002f;
            } else {
              sum += -0.099282443523f;
            }
          }
        }
      } else {
        if (!(data[2].missing != -1) || (data[2].qvalue < 116)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 38)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 92)) {
              sum += -0.24246165156f;
            } else {
              sum += 0.23805081844f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 38)) {
              sum += -0.061675626785f;
            } else {
              sum += 0.16984757781f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 70)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 94)) {
              sum += 0.15629598498f;
            } else {
              sum += -0.35711985826f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 144)) {
              sum += 0.32307493687f;
            } else {
              sum += -0.21315024793f;
            }
          }
        }
      }
    } else {
      if (!(data[7].missing != -1) || (data[7].qvalue < 34)) {
        if (!(data[5].missing != -1) || (data[5].qvalue < 64)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 52)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 176)) {
              sum += 0.22842991352f;
            } else {
              sum += -0.12660010159f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 26)) {
              sum += 0.33111020923f;
            } else {
              sum += 0.029794618487f;
            }
          }
        } else {
          if (!(data[7].missing != -1) || (data[7].qvalue < 0)) {
            sum += -0.12871032953f;
          } else {
            sum += -0.44409167767f;
          }
        }
      } else {
        if (!(data[5].missing != -1) || (data[5].qvalue < 24)) {
          if (!(data[7].missing != -1) || (data[7].qvalue < 92)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 80)) {
              sum += 0.25316432118f;
            } else {
              sum += -0.12411771715f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 62)) {
              sum += -0.20696249604f;
            } else {
              sum += 0.1336889714f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 120)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 64)) {
              sum += 0.11274586618f;
            } else {
              sum += -0.10939381272f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 126)) {
              sum += -0.55750131607f;
            } else {
              sum += -0.18061569333f;
            }
          }
        }
      }
    }
  } else {
    if (!(data[0].missing != -1) || (data[0].qvalue < 58)) {
      if (!(data[4].missing != -1) || (data[4].qvalue < 32)) {
        if (!(data[1].missing != -1) || (data[1].qvalue < 96)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 40)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 84)) {
              sum += 0.023038230836f;
            } else {
              sum += -0.24926804006f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 78)) {
              sum += 0.16591928899f;
            } else {
              sum += -0.19828000665f;
            }
          }
        } else {
          if (!(data[5].missing != -1) || (data[5].qvalue < 132)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 38)) {
              sum += -0.10259795189f;
            } else {
              sum += -0.26523378491f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 104)) {
              sum += 0.56933486462f;
            } else {
              sum += -0.18348355591f;
            }
          }
        }
      } else {
        if (!(data[6].missing != -1) || (data[6].qvalue < 50)) {
          if (!(data[1].missing != -1) || (data[1].qvalue < 110)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 66)) {
              sum += -0.23956920207f;
            } else {
              sum += 0.065431877971f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 136)) {
              sum += -0.22523187101f;
            } else {
              sum += 0.18363089859f;
            }
          }
        } else {
          if (!(data[1].missing != -1) || (data[1].qvalue < 80)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 94)) {
              sum += -0.097974978387f;
            } else {
              sum += -0.3172917366f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 124)) {
              sum += -0.13013194501f;
            } else {
              sum += -0.25376486778f;
            }
          }
        }
      }
    } else {
      if (!(data[2].missing != -1) || (data[2].qvalue < 136)) {
        if (!(data[0].missing != -1) || (data[0].qvalue < 146)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 128)) {
            if (!(data[3].missing != -1) || (data[3].qvalue < 0)) {
              sum += -0.27256649733f;
            } else {
              sum += 0.03179179877f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 24)) {
              sum += 0.26812791824f;
            } else {
              sum += 0.12670950592f;
            }
          }
        } else {
          if (!(data[4].missing != -1) || (data[4].qvalue < 58)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 62)) {
              sum += -0.22099395096f;
            } else {
              sum += 0.15049368143f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 76)) {
              sum += 0.094258747995f;
            } else {
              sum += -0.20526857674f;
            }
          }
        }
      } else {
        if (!(data[5].missing != -1) || (data[5].qvalue < 74)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 146)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 66)) {
              sum += 0.058812357485f;
            } else {
              sum += -0.24799342453f;
            }
          } else {
            if (!(data[3].missing != -1) || (data[3].qvalue < 10)) {
              sum += -0.14941608906f;
            } else {
              sum += 0.193966344f;
            }
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 154)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 40)) {
              sum += -0.33717817068f;
            } else {
              sum += -0.1591116786f;
            }
          } else {
            sum += 0.3470184207f;
          }
        }
      }
    }
  }
  if (!(data[1].missing != -1) || (data[1].qvalue < 28)) {
    if (!(data[7].missing != -1) || (data[7].qvalue < 22)) {
      if (!(data[3].missing != -1) || (data[3].qvalue < 26)) {
        if (!(data[1].missing != -1) || (data[1].qvalue < 2)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 88)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 36)) {
              sum += -0.12528465688f;
            } else {
              sum += 0.25399044156f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 134)) {
              sum += 0.17999695241f;
            } else {
              sum += -0.94692373276f;
            }
          }
        } else {
          if (!(data[4].missing != -1) || (data[4].qvalue < 2)) {
            sum += -0.25266212225f;
          } else {
            if (!(data[3].missing != -1) || (data[3].qvalue < 8)) {
              sum += -0.12342580408f;
            } else {
              sum += 0.29052004218f;
            }
          }
        }
      } else {
        if (!(data[2].missing != -1) || (data[2].qvalue < 56)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 46)) {
            sum += 0.098419576883f;
          } else {
            sum += -0.40381368995f;
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 102)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 112)) {
              sum += 0.29525926709f;
            } else {
              sum += -0.098691917956f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 112)) {
              sum += -0.39547210932f;
            } else {
              sum += 0.18901559711f;
            }
          }
        }
      }
    } else {
      if (!(data[5].missing != -1) || (data[5].qvalue < 20)) {
        if (!(data[7].missing != -1) || (data[7].qvalue < 90)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 4)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 2)) {
              sum += 0.098636955023f;
            } else {
              sum += -0.38988566399f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 156)) {
              sum += 0.29859343171f;
            } else {
              sum += 0.17874911427f;
            }
          }
        } else {
          if (!(data[5].missing != -1) || (data[5].qvalue < 4)) {
            if (!(data[4].missing != -1) || (data[4].qvalue < 56)) {
              sum += 0.23610378802f;
            } else {
              sum += -0.024616613984f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 66)) {
              sum += -0.18279320002f;
            } else {
              sum += 0.20552070439f;
            }
          }
        }
      } else {
        if (!(data[2].missing != -1) || (data[2].qvalue < 108)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 92)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 68)) {
              sum += 0.14815534651f;
            } else {
              sum += -0.079760506749f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 64)) {
              sum += -0.15013444424f;
            } else {
              sum += 0.065601669252f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 16)) {
            if (!(data[3].missing != -1) || (data[3].qvalue < 22)) {
              sum += -0.49276807904f;
            } else {
              sum += 0.02729714103f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 76)) {
              sum += 0.13862106204f;
            } else {
              sum += 0.287946105f;
            }
          }
        }
      }
    }
  } else {
    if (!(data[0].missing != -1) || (data[0].qvalue < 44)) {
      if (!(data[6].missing != -1) || (data[6].qvalue < 106)) {
        if (!(data[1].missing != -1) || (data[1].qvalue < 126)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 118)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 112)) {
              sum += -0.042982768267f;
            } else {
              sum += -0.15623581409f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 30)) {
              sum += 0.28545472026f;
            } else {
              sum += -0.0043401927687f;
            }
          }
        } else {
          sum += -0.30993691087f;
        }
      } else {
        if (!(data[2].missing != -1) || (data[2].qvalue < 24)) {
          if (!(data[3].missing != -1) || (data[3].qvalue < 28)) {
            sum += -0.3167937398f;
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 62)) {
              sum += -0.16834384203f;
            } else {
              sum += 0.073857240379f;
            }
          }
        } else {
          if (!(data[1].missing != -1) || (data[1].qvalue < 86)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 110)) {
              sum += -0.1542481184f;
            } else {
              sum += 0.26606094837f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 14)) {
              sum += -0.31460478902f;
            } else {
              sum += -0.18714016676f;
            }
          }
        }
      }
    } else {
      if (!(data[1].missing != -1) || (data[1].qvalue < 88)) {
        if (!(data[6].missing != -1) || (data[6].qvalue < 138)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 2)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 166)) {
              sum += 0.15316866338f;
            } else {
              sum += -0.23465041816f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 142)) {
              sum += 0.023652341217f;
            } else {
              sum += -0.067758157849f;
            }
          }
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 128)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 148)) {
              sum += 0.16866861284f;
            } else {
              sum += -0.2481456697f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 188)) {
              sum += -0.20808319747f;
            } else {
              sum += 0.14341172576f;
            }
          }
        }
      } else {
        if (!(data[1].missing != -1) || (data[1].qvalue < 116)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 38)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 102)) {
              sum += 0.68878304958f;
            } else {
              sum += 0.26692062616f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 62)) {
              sum += -0.044355276972f;
            } else {
              sum += -0.16577248275f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 18)) {
            if (!(data[3].missing != -1) || (data[3].qvalue < 34)) {
              sum += -0.23266935349f;
            } else {
              sum += 0.33745315671f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 8)) {
              sum += -0.072235919535f;
            } else {
              sum += -0.30752471089f;
            }
          }
        }
      }
    }
  }
  if (!(data[0].missing != -1) || (data[0].qvalue < 80)) {
    if (!(data[0].missing != -1) || (data[0].qvalue < 2)) {
      if (!(data[7].missing != -1) || (data[7].qvalue < 86)) {
        if (!(data[2].missing != -1) || (data[2].qvalue < 10)) {
          sum += -0.29916197062f;
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 18)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 14)) {
              sum += -0.11070735753f;
            } else {
              sum += 0.39779490232f;
            }
          } else {
            if (!(data[7].missing != -1) || (data[7].qvalue < 4)) {
              sum += 0.027016468346f;
            } else {
              sum += -0.2599363327f;
            }
          }
        }
      } else {
        if (!(data[6].missing != -1) || (data[6].qvalue < 72)) {
          if (!(data[1].missing != -1) || (data[1].qvalue < 72)) {
            sum += -0.19811989367f;
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 0)) {
              sum += 0.44816160202f;
            } else {
              sum += -0.042731717229f;
            }
          }
        } else {
          if (!(data[3].missing != -1) || (data[3].qvalue < 24)) {
            sum += -0.25610673428f;
          } else {
            sum += 0.019007919356f;
          }
        }
      }
    } else {
      if (!(data[1].missing != -1) || (data[1].qvalue < 120)) {
        if (!(data[2].missing != -1) || (data[2].qvalue < 140)) {
          if (!(data[4].missing != -1) || (data[4].qvalue < 52)) {
            if (!(data[1].missing != -1) || (data[1].qvalue < 42)) {
              sum += -0.28665053844f;
            } else {
              sum += 0.032655201852f;
            }
          } else {
            if (!(data[2].missing != -1) || (data[2].qvalue < 78)) {
              sum += -0.12290850282f;
            } else {
              sum += 0.069752879441f;
            }
          }
        } else {
          if (!(data[6].missing != -1) || (data[6].qvalue < 44)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 62)) {
              sum += 0.14114427567f;
            } else {
              sum += -0.20257011056f;
            }
          } else {
            if (!(data[4].missing != -1) || (data[4].qvalue < 74)) {
              sum += -0.17620423436f;
            } else {
              sum += 0.065727561712f;
            }
          }
        }
      } else {
        if (!(data[0].missing != -1) || (data[0].qvalue < 34)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 138)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 40)) {
              sum += 0.093124002218f;
            } else {
              sum += -0.25657606125f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 64)) {
              sum += 0.39393892884f;
            } else {
              sum += -0.1950751245f;
            }
          }
        } else {
          if (!(data[3].missing != -1) || (data[3].qvalue < 62)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 6)) {
              sum += -0.089689001441f;
            } else {
              sum += -0.30616483092f;
            }
          } else {
            sum += -0.016913361847f;
          }
        }
      }
    }
  } else {
    if (!(data[5].missing != -1) || (data[5].qvalue < 26)) {
      if (!(data[0].missing != -1) || (data[0].qvalue < 124)) {
        if (!(data[6].missing != -1) || (data[6].qvalue < 28)) {
          if (!(data[5].missing != -1) || (data[5].qvalue < 2)) {
            if (!(data[2].missing != -1) || (data[2].qvalue < 84)) {
              sum += 0.023177988827f;
            } else {
              sum += -0.42493262887f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 24)) {
              sum += 0.22754983604f;
            } else {
              sum += -0.13193815947f;
            }
          }
        } else {
          if (!(data[3].missing != -1) || (data[3].qvalue < 4)) {
            sum += 0.033530715853f;
          } else {
            sum += 0.2953633368f;
          }
        }
      } else {
        if (!(data[7].missing != -1) || (data[7].qvalue < 80)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 22)) {
            if (!(data[4].missing != -1) || (data[4].qvalue < 70)) {
              sum += -0.37699499726f;
            } else {
              sum += 0.094854287803f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 6)) {
              sum += 0.31056261063f;
            } else {
              sum += 0.17056725919f;
            }
          }
        } else {
          if (!(data[0].missing != -1) || (data[0].qvalue < 190)) {
            if (!(data[6].missing != -1) || (data[6].qvalue < 62)) {
              sum += -0.17411699891f;
            } else {
              sum += 0.074073821306f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 114)) {
              sum += 0.37906756997f;
            } else {
              sum += 0.087031722069f;
            }
          }
        }
      }
    } else {
      if (!(data[7].missing != -1) || (data[7].qvalue < 12)) {
        if (!(data[0].missing != -1) || (data[0].qvalue < 180)) {
          if (!(data[2].missing != -1) || (data[2].qvalue < 92)) {
            if (!(data[3].missing != -1) || (data[3].qvalue < 16)) {
              sum += -0.20338785648f;
            } else {
              sum += 0.27142757177f;
            }
          } else {
            if (!(data[1].missing != -1) || (data[1].qvalue < 38)) {
              sum += 0.14377754927f;
            } else {
              sum += -0.04760298878f;
            }
          }
        } else {
          if (!(data[5].missing != -1) || (data[5].qvalue < 66)) {
            if (!(data[5].missing != -1) || (data[5].qvalue < 58)) {
              sum += -0.34350389242f;
            } else {
              sum += 0.23436897993f;
            }
          } else {
            if (!(data[6].missing != -1) || (data[6].qvalue < 142)) {
              sum += -0.31073150039f;
            } else {
              sum += 0.11632180214f;
            }
          }
        }
      } else {
        if (!(data[6].missing != -1) || (data[6].qvalue < 86)) {
          if (!(data[6].missing != -1) || (data[6].qvalue < 10)) {
            if (!(data[7].missing != -1) || (data[7].qvalue < 84)) {
              sum += -0.24812029302f;
            } else {
              sum += 0.045689426363f;
            }
          } else {
            if (!(data[0].missing != -1) || (data[0].qvalue < 100)) {
              sum += 0.013575413264f;
            } else {
              sum += 0.12595304847f;
            }
          }
        } else {
          if (!(data[2].missing != -1) || (data[2].qvalue < 120)) {
            if (!(data[0].missing != -1) || (data[0].qvalue < 82)) {
              sum += 0.15265879035f;
            } else {
              sum += -0.087630487978f;
            }
          } else {
            if (!(data[5].missing != -1) || (data[5].qvalue < 68)) {
              sum += 0.25061047077f;
            } else {
              sum += 0.012460231781f;
            }
          }
        }
      }
    }
  }

  return 1.0f / (1 + expf(-sum));
}

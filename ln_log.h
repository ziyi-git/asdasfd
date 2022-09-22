#include <stdio.h>  
#include <stdarg.h>

#define LN_LOG(...) { \
	do { if (lnLog != NULL) {lnLog(__VA_ARGS__); }} while(0); \
}

#include "toothbrush_position_detection.h"

extern tpdLogCallback_t lnLog;

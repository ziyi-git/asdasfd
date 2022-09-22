#ifndef __LN_ALG_MEM_H__
#define __LN_ALG_MEM_H__

#pragma once

#include <stdint.h>

typedef void* (*lnAlgMallocCallback_t)(uint32_t size);
typedef void (*lnAlgFreeCallback_t)(void *ptr);

extern lnAlgMallocCallback_t lnAlgMalloc;
extern lnAlgFreeCallback_t lnAlgFree;

#endif  /**< __LN_ALG_MEMORY_H__. */

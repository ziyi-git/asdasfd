#include "ln_alg_mem.h"
#include "ln_heap.h"

// tpdMallocCallback_t tpdMalloc = ln_malloc;
// tpdFreeCallback_t tpdFree = ln_free;

lnAlgMallocCallback_t lnAlgMalloc = (lnAlgMallocCallback_t)ln_malloc;
lnAlgFreeCallback_t lnAlgFree = (lnAlgFreeCallback_t)ln_free;
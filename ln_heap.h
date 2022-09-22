
#ifndef __LN_HEAP_H__
#define __LN_HEAP_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void *ln_malloc( size_t xSize );
void ln_free( void *pv );
size_t ln_get_free_heap_size( void );
size_t ln_get_min_ever_free_heap_size( void );

#ifdef __cplusplus
}
#endif

#endif /* __LN_HEAP_H__ */


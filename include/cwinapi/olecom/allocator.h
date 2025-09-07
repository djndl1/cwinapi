#ifndef OLE_ALLOCATOR_H
#define OLE_ALLOCATOR_H

#include "cwinapi/cwinapi_internal/compilers.h"
#include "modernlib/allocator.h"

#ifdef __cplusplus
extern "C" {
#endif


CWINAPI_PUBLIC
extern const mem_allocator *const com_allocator;

#define ole_allocator com_allocator


#ifdef __cplusplus
}
#endif


#endif // OLE_ALLOCATOR_H

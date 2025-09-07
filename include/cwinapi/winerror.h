#ifndef WINERROR_H_
#define WINERROR_H_

#include <windows.h>
#include <stdint.h>

#include <stdbool.h>
#include "cwinapi_internal/compilers.h"

#include "modernlib/dyn_cwstr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct winerror {
    uint32_t code;
} winerror;

#ifdef __cplusplus
#define WINERROR(e) winerror{ e }
#else
#define WINERROR(e) ((winerror){ .code = e })
#endif


typedef struct winstatus {
    winerror error;
    bool succeeded;
} winstatus;

#define WINSTATUS(err, ok) ((winstatus) { .error = err , .succeeded = ok })
#define WIN_OK ((winstatus){ .succeeded = true })
#define WIN_ERR(e) WINSTATUS(e, false)
#define WIN_LASTERR WIN_ERR(last_error())

CWINAPI_PUBLIC
winerror last_error();

CWINAPI_PUBLIC
dyn_cwstr_result winerror_get_message(winerror error, const mem_allocator *const allocator);

#ifdef __cplusplus
}
#endif

#endif // WINERROR_H_

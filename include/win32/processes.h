#pragma once

#include <windows.h>
#include <stdint.h>
#include <cwinapi/cwinapi_internal/compilers.h>

#if defined (__cplusplus)
extern "C" {
#endif

CWINAPI_PUBLIC
uint32_t create_process(const wchar_t*, const wchar_t**, LPPROCESS_INFORMATION process);

#if defined (__cplusplus)
}
#endif

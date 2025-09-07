#ifndef WINFILE_H_
#define WINFILE_H_

#include <windows.h>
#include "winhandle.h"
#include <modernlib/data_buffer.h>
#include <stdint.h>
#include "cwinapi_internal/compilers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct winfile_open_request {
    uint32_t requested_access;
    uint32_t share_mode;
    bool handle_inheritable;
    SECURITY_DESCRIPTOR *security_descriptor;
    uint32_t creation_disposition;
    uint32_t flags_attributes;

    winhandle template_file;
} winfile_open_request;

typedef struct winfile_open_result {
    winstatus status;
    winhandle file_handle;
} winfile_open_result;

CWINAPI_PUBLIC
winfile_open_result winfile_open(const wchar_t *filename, const winfile_open_request request);

typedef struct winfile_read_result {
    winstatus status;
    uint32_t read_count;
} winfile_read_result;

CWINAPI_PUBLIC
winfile_read_result winfile_sync_read_into(const winhandle fh,
                                           const data_buffer buffer);

typedef struct winfile_read_result_buffer {
    winstatus status;
    data_buffer buffer;
    uint32_t read_count;
} winfile_read_result_buffer;

CWINAPI_PUBLIC
winfile_read_result_buffer winfile_sync_read_as_buffer(const winhandle fh,
                                                       uint32_t count,
                                                       const mem_allocator *allocator);

typedef struct winfile_write_result {
    winstatus status;
    uint32_t written_count;
} winfile_write_result;

CWINAPI_PUBLIC
winfile_write_result winfile_sync_write(const winhandle self, const data_buffer buf, uint32_t write_count);

#ifdef __cplusplus
}
#endif

#endif // WINFILE_H_

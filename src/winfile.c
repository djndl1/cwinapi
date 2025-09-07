#include "cwinapi/winfile.h"

winfile_open_result winfile_open(const wchar_t *filename,
                                 const winfile_open_request request)
{
    SECURITY_ATTRIBUTES security_attr = {
        .nLength = sizeof(SECURITY_ATTRIBUTES),
        .lpSecurityDescriptor = request.security_descriptor,
        .bInheritHandle = request.handle_inheritable,
    };

    winhandle fh = WINHANDLE(CreateFile(filename,
                                        request.requested_access,
                                        request.share_mode,
                                        &security_attr,
                                        request.creation_disposition,
                                        request.flags_attributes,
                                        request.template_file.handle));

    if (winhandle_invalid(fh)) {
        return (winfile_open_result) {
            .file_handle = fh,
            .status = WIN_LASTERR,
        };
    }

    return (winfile_open_result){
         .file_handle = fh,
         .status = WIN_OK,
    };
}

winfile_read_result winfile_sync_read_into(const winhandle fh,
                                           const data_buffer buffer)
{
    DWORD read_count = 0;
    if (ReadFile(fh.handle, buffer.data, buffer.length,
                 &read_count, NULL)) {
        return (winfile_read_result){
            .status = WIN_OK,
            .read_count = read_count,
        };
    }

    return (winfile_read_result){
            .status = WIN_LASTERR,
            .read_count = 0,
    };
}

winfile_read_result_buffer winfile_sync_read_as_buffer(const winhandle fh,
                                                       uint32_t count,
                                                       const mem_allocator *allocator)
{
    buffer_alloc_result alloc_res = data_buffer_new(count, allocator);
    if (alloc_res.error) {
        return (winfile_read_result_buffer){
            .status = WINSTATUS(alloc_res.error, false),
        };
    }
    data_buffer buffer = alloc_res.buffer;

    DWORD read_count = 0;
    if (ReadFile(fh.handle, buffer.data, count,
                 &read_count, NULL)) {
        return (winfile_read_result_buffer){
            .status = WIN_OK,
            .buffer = buffer,
            .read_count = read_count,
        };
    }

    data_buffer_destroy(&buffer);
    return (winfile_read_result_buffer){
            .status = WIN_LASTERR,
            .read_count = 0,
    };
}

winfile_write_result winfile_sync_write(const winhandle self, const data_buffer buf, uint32_t write_count)
{
    DWORD nbyte_write = write_count == 0 ? buf.length : MIN(write_count, buf.length);

    DWORD written_count = 0;
    if (!WriteFile(self.handle, buf.data, nbyte_write, &written_count, NULL)) {
        return (winfile_write_result){
            .status = WIN_LASTERR,
            .written_count = 0,
        };
    }

    return (winfile_write_result){
          .status = WIN_OK,
          .written_count = written_count,
    };
}

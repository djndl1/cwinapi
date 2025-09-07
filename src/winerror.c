#include "cwinapi/winerror.h"
#include "modernlib/dyn_cwstr.h"

winerror last_error()
{
    DWORD err = GetLastError();

    return (winerror) { .code = err };
}

dyn_cwstr_result winerror_get_message(winerror error, const mem_allocator *const allocator)
{
    wchar_t *message;
    uint32_t num = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error.code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (wchar_t*)&message,
        0,
        nullptr);

    if (num == 0) {
        return (dyn_cwstr_result){
            .error = last_error().code,
        };
    } else {
        const wchar_t *msg = message;
        dyn_cwstr_result msg_result = dyn_cwstr_from_nts(msg, allocator);
success_ret:
        LocalFree(message);
        return msg_result;
    }
}

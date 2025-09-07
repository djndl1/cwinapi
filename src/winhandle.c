#include "cwinapi/winhandle.h"

winhandle_info_result winhandle_info(winhandle handle)
{
    DWORD flags = 0;
    if (GetHandleInformation(handle.handle, &flags)) {
        return (winhandle_info_result){
            .status = WIN_OK,
            .inheritable = flags & HANDLE_FLAG_INHERIT,
            .protected_from_close = flags & HANDLE_FLAG_PROTECT_FROM_CLOSE,
        };
    } else {
        return (winhandle_info_result){
            .status = WIN_LASTERR,
        };
    }
}

static winstatus set_winhandle_flag(winhandle self, DWORD mask, bool bit)
{
    if (SetHandleInformation(self.handle, mask, bit)) {
        return WIN_OK;
    } else {
        return WIN_LASTERR;
    }

}

winstatus winhandle_set_inheritable(winhandle self, bool inheritable)
{
    return set_winhandle_flag(self, HANDLE_FLAG_INHERIT, inheritable);
}

winstatus winhandle_set_protected_from_close(winhandle self, bool inheritable)
{
    return set_winhandle_flag(self, HANDLE_FLAG_PROTECT_FROM_CLOSE, inheritable);
}

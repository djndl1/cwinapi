
//#include <windows.h>

#include <stdio.h>
#include <cwinapi/winfile.h>
#include <cwinapi/winhandle.h>


#include "utest.h"

UTEST(WIN32, NULL_BUFFER)
{
    winfile_open_result open_result = winfile_open(L"abc.txt", (winfile_open_request){
            .requested_access = GENERIC_READ | GENERIC_WRITE,
            .creation_disposition = CREATE_ALWAYS,
            .share_mode = FILE_SHARE_READ,
            .flags_attributes = FILE_ATTRIBUTE_NORMAL,
        });

    if (!open_result.status.succeeded && winhandle_invalid(open_result.file_handle)) {
        fprintf(stdout, "error opening abc.txt: 0x%x\n", open_result.status.error.code);
        return;
    }
    winhandle h = open_result.file_handle;

    winfile_read_result read_result = winfile_sync_read_into(h, (data_buffer){ .data = NULL, .length = 1 });
    if (!read_result.status.succeeded) {
        fprintf(stdout, "error reading abc.txt into null buffer: 0x%x\n", read_result.status.error.code);
        EXPECT_EQ(read_result.status.error.code, 0x3e6);
    }

    winfile_write_result write_result = winfile_sync_write(h, (data_buffer){ .data = NULL, .length = 1 }, 1);
    if (!write_result.status.succeeded) {
        fprintf(stdout, "error writing null buffer to abc.txt: 0x%x\n", write_result.status.error.code);
        EXPECT_EQ(write_result.status.error.code, 0x6f8);
        return;
    }

close_file:
    if (!winhandle_invalid(h)) {
        winhandle_close(&h);
    }
}


UTEST_MAIN();

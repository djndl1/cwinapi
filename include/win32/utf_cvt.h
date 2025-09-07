#include <exception>
#include <cwinapi/cwinapi_internal/compilers.h>
#include <string>

namespace utf_cvt {
    class CWINAPI_PUBLIC utf_conversion_error;

    CWINAPI_PUBLIC
    std::wstring utf8_to_utf16(const std::string &utf8_string);
}

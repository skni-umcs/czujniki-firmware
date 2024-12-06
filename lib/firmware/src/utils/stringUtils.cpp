#include "stringUtils.h"
#include <algorithm>

std::string fromWString(String wString) {
    std::string output;
    std::transform(wString.begin(), wString.end(),
        std::back_insert_iterator<std::string>(output),
        [](wchar_t wchar)
        {
            return static_cast<char>(wchar > 127 ? '?' : wchar);
        }
    );
    return output;
}
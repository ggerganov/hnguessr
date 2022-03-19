#include "utils.h"

void Utils::replaceAll(std::string & s, const std::string & search, const std::string & replace) {
    for (size_t pos = 0; ; pos += replace.length()) {
        pos = s.find(search, pos);
        if (pos == std::string::npos) break;
        s.erase(pos, search.length());
        s.insert(pos, replace);
    }
}

std::string Utils::timestampFormat(int64_t timestamp_s, const char * format) {
    time_t t = time_t(timestamp_s);
    struct tm * tm = gmtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), format, tm);
    return buf;
}

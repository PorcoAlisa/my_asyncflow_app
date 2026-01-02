#include "comm.h"
#include <sstream>

std::string ReplaceString(std::string_view subject, std::string_view search, std::string_view replace) {
    if (search.empty()) return std::string(subject);

    std::string out;
    out.reserve(subject.size());

    std::size_t last = 0;
    while (true) {
        std::size_t pos = subject.find(search, last);
        if (pos == std::string_view::npos) break;
        out.append(subject.substr(last, pos - last));
        out.append(replace);

        last = pos + search.size();
    }
    out.append(subject.substr(last));
    return out;
}

std::vector<std::string_view> StringSplit(std::string_view s, const char delim) {
    std::vector<std::string_view> sv;
    sv.reserve(3);
    size_t start = 0;
    size_t end = s.find(delim);

    while (end != std::string_view::npos) {
        sv.emplace_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delim, start);
    }

    if (start < s.size()) {
        sv.emplace_back(s.substr(start));
    }
    return sv;
}

std::string StringJoin(const std::vector<std::string>& v, const char c) {
    std::string out;
    for (size_t i = 0; i < v.size(); ++i) {
        out += v[i];
        if (i + 1 < v.size()) out += c;
    }
    return out;
}

std::string generate_uuid_v4() {
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    }
    return ss.str();
}

uint32_t TimestampNow() {
  const unsigned long int sec = time(nullptr);
  return sec;
}
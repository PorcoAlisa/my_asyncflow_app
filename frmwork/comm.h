#pragma once

#include <random>
#include <google/protobuf/util/json_util.h>

template <typename... Args>
std::string Sprintf(const std::string& format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    if (size_s <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return {buf.get(), buf.get() + size - 1};
}

std::string ReplaceString(std::string_view subject, std::string_view search, std::string_view replace);
std::vector<std::string_view> StringSplit(std::string_view s, char delim);
std::string StringJoin(const std::vector<std::string>& v, char c);

std::string generate_uuid_v4();
uint32_t TimestampNow();
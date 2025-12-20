#pragma once

#include <random>
#include <google/protobuf/util/json_util.h>

namespace Protobuf = google::protobuf;
namespace ProtobufUtil = google::protobuf::util;

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
static std::uniform_int_distribution<> dis2(8, 11);

template <typename... Args>
std::string Sprintf(const std::string& format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    if (size_s <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    size_t size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

std::string ReplaceString(std::string_view subject, std::string_view search, std::string_view replace);
std::vector<std::string_view> StringSplit(std::string_view s, const char delim);
std::string StringJoin(const std::vector<std::string>& v, const char c);

std::string generate_uuid_v4();
uint32_t TimestampNow();
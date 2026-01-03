#pragma once

#include <string>

namespace async_flow::frmwork {

enum TaskStatus { TASK_PENDING, TASK_PROCESSING, TASK_SUCC, TASK_FAILED };
enum Error { OK, FAILED };

class Status {
public:
    Status();
    Status(int error_code, std::string error_message):error_code_(error_code), error_message_(std::move(error_message)) {}
    Status(const Status&);
    ~Status() = default;

    static const Status OK;
    static const Status FAIL;

    void set(int code, std::string msg) {
        error_code_ = code;
        error_message_ = std::move(msg);
    }

    [[nodiscard]] bool ok() const { return error_code_ == Error::OK; }
    [[nodiscard]] int error_code() const { return error_code_; }
    [[nodiscard]] std::string error_message() const { return error_message_; }

    bool operator==(const Status& x) const { return x.error_code() == error_code_; }
    bool operator!=(const Status& x) const { return !operator==(x); }

private:
    int error_code_;
    std::string error_message_;
};

}
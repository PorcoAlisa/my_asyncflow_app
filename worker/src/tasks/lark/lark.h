#pragma once

#include "task.h"

namespace async_flow::worker {

class Lark : public TaskBase {
public:
    Lark(api::TaskData data, std::string_view host) : TaskBase(std::move(data), host) {}
    drogon::Task<frmwork::Status> HandleProcess() override {
        taskData_.set_status(frmwork::TASK_SUCC);
        co_return frmwork::Status::OK;
    }
    static TaskPtr CreateLark(api::TaskData data, std::string_view host);
};

}

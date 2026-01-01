#pragma once

#include "task.h"

namespace async_flow::worker {

class Lark : public TaskBase {
public:
    Lark(const api::TaskData& data, std::string_view host) : TaskBase(data, host) {}

    static TaskPtr CreateLark(const api::TaskData& data, std::string_view host);
};

}

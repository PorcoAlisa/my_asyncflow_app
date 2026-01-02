#pragma once

#include "task.h"

namespace async_flow::worker {

class Lark : public TaskBase {
public:
    Lark(api::TaskData data, std::string_view host) : TaskBase(std::move(data), host) {}

    static TaskPtr CreateLark(api::TaskData data, std::string_view host);
};

}

#pragma once

#include "api.pb.h"
#include <functional>

namespace async_flow {
namespace worker {


class TaskBase {
public:
    TaskBase() {}
    TaskBase(const api::TaskData& taskData, std::string_view host) : taskData_(taskData), svr_host_(host) {}
    virtual ~TaskBase() = default;
protected:
    api::TaskData taskData_;
    std::string svr_host_;
};

using TaskPtr = std::shared_ptr<TaskBase>;
using TaskBaseFactory = std::function<TaskPtr(const api::TaskData&, std::string_view)>;

}
}
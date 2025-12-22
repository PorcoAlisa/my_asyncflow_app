#pragma once

#include <string>
#include <string_view>
#include "task.h"

namespace async_flow {
namespace worker {

class TaskMgr {
public:
    TaskMgr(std::string_view taskType, std::string_view host, TaskBaseFactory factory)
        : taskType_(taskType), taskSvrHost_(host), factory_(std::move(factory)) {}
    void Init();
private:
    std::string taskType_;
    std::string taskSvrHost_;
    TaskBaseFactory factory_;
};

}
}
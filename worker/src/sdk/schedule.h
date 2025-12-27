#pragma once

#include <string>
#include <string_view>
#include "task.h"
#include <thread>
#include "api.pb.h"

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
    // std::jthread loadCfgJt_;
    std::map<std::string, api::TaskScheduleCfg> cfgMap_;
};

}
}
#pragma once

#include <string>
#include <string_view>
#include "task.h"
#include "api.pb.h"

namespace async_flow::worker {

class TaskMgr : public std::enable_shared_from_this<TaskMgr> {
public:
    TaskMgr(std::string_view taskType, std::string_view host, TaskBaseFactory factory) :
        taskType_(taskType), taskSvrHost_(host), factory_(std::move(factory)) {}
    void Init();
    drogon::Task<> Schedule();
    drogon::Task<std::vector<TaskPtr>> Hold(const api::TaskScheduleCfg& cfg);
    static drogon::Task<> RunTask(api::TaskScheduleCfg cfg, TaskPtr taskPtr);
private:
    std::string taskType_;
    std::string taskSvrHost_;
    TaskBaseFactory factory_;
    std::mutex lock_;
    std::map<std::string, api::TaskScheduleCfg> cfgMap_;
    drogon::Task<> LoadCfgLoop();
};

}

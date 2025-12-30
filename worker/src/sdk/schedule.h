#pragma once

#include <string>
#include <string_view>
#include "task.h"
#include <thread>
#include "api.pb.h"

namespace async_flow {
namespace worker {

class TaskMgr : public std::enable_shared_from_this<TaskMgr> {
public:
    TaskMgr(std::string_view taskType, std::string_view host)
        : taskType_(taskType), taskSvrHost_(host) {}
    void Init();
    void Schedule();
    drogon::Task<std::vector<TaskPtr>> Hold(const api::TaskScheduleCfg& cfg);
private:
    std::string taskType_;
    std::string taskSvrHost_;
    TaskBaseFactory factory_;
    // std::jthread loadCfgJt_;
    std::mutex lock_;
    std::map<std::string, api::TaskScheduleCfg> cfgMap_;
    drogon::Task<void> LoadCfgLoop();
};

}
}
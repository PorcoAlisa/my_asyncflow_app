#include "schedule.h"
#include <drogon/drogon.h>

namespace async_flow {
namespace worker {

using namespace frmwork;

drogon::Task<void> TaskMgr::LoadCfgLoop() {
    auto* taskRpc = drogon::app().getPlugin<TaskRpc>();
    if (!taskRpc) {
        LOG_ERROR << "TaskRpc plugin not found! Did you add it to config.json?";
        co_return;
    }
    api::GetTaskScheduleCfgListReq req;
    auto resultPair = co_await taskRpc->GetTaskScheduleCfgList(req);
    if (!resultPair.second.ok()) {
        // 目前错误返回的都是FAIL，error_code都是一样的，这里没啥意义
        LOG_ERROR << "GetTaskScheduleCfgList err: " << resultPair.second.error_code();
        co_return;
    }
    std::map<std::string, api::TaskScheduleCfg> newMap;
    api::GetTaskScheduleCfgListRsp& rsp = resultPair.first;
    LOG_INFO << "schedule cfg list size: " << rsp.task_schedule_cfg_size();
    for (int i = 0; i < rsp.task_schedule_cfg_size(); ++i) {
        newMap[rsp.task_schedule_cfg(i).task_type()] = rsp.task_schedule_cfg(i);
    }
    {
        std::lock_guard<std::mutex> guard(lock_);
        cfgMap_ = newMap;
    }
    LOG_INFO << "update task schedule done";
}

void TaskMgr::Init() {
    LOG_INFO << "TaskMgr Init";

    auto self = shared_from_this();
    drogon::async_run([self]() -> drogon::Task<void> {
        return self->LoadCfgLoop();
    });
}

}
}
#include "schedule.h"
#include <drogon/drogon.h>

namespace async_flow {
namespace worker {

using namespace frmwork;

drogon::Task<void> LoadCfgLoop() {
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
    
    
}

void TaskMgr::Init() {
    LOG_INFO << "Init";


}

}
}
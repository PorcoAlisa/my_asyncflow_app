#include "get_task_schedule_cfg_list.h"
#include "const.h"
#include "schedule_cfg.h"

namespace async_flow {
namespace flowsvr {

using namespace frmwork;
using namespace db;

Task<async_flow::frmwork::Status> GetTaskScheduleCfgListHandler::HandleInput(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody) {
    co_return Status::OK;
}

Task<std::pair<api::GetTaskScheduleCfgListRsp, Status>> GetTaskScheduleCfgListHandler::HandleProcess(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody) {
    ScheduleCfgDao posDao;
    auto [cfgList, status] = co_await posDao.GetListAsync();
    if (!status.ok()) co_return {{}, status};

    api::GetTaskScheduleCfgListRsp rspBody;
    for (const auto& cfg : cfgList) {
        api::TaskScheduleCfg* apiCfg = rspBody.add_task_schedule_cfg();
        FillDBScheduleCfgModel(cfg, *apiCfg);
    }

    co_return {std::move(rspBody), Status::OK};
}

}
}
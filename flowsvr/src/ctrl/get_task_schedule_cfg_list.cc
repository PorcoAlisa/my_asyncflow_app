#include "get_task_schedule_cfg_list.h"
#include "const.h"

namespace async_flow {
namespace flowsvr {

using namespace frmwork;

Task<async_flow::frmwork::Status> GetTaskScheduleCfgListHandler::HandleInput(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody) {
    co_return Status::OK;
}

Task<std::pair<api::GetTaskScheduleCfgListRsp, Status>> GetTaskScheduleCfgListHandler::HandleProcess(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody) {
    co_return {{}, Status::OK};
}

}
}
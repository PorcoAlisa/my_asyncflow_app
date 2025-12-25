#include "task.h"

namespace async_flow {
namespace worker {

drogon::Task<std::pair<api::GetTaskScheduleCfgListRsp, async_flow::frmwork::Status>> TaskRpc::GetTaskScheduleCfgList(const api::GetTaskScheduleCfgListReq& req) {
    co_return co_await SendRequest<api::GetTaskScheduleCfgListReq, api::GetTaskScheduleCfgListRsp>("/get_task_schedule_cfg_list", req);
}

drogon::Task<std::pair<api::HoldTasksRsp, frmwork::Status>> TaskRpc::HoldTasks(const api::HoldTasksReq& req) {
    co_return co_await SendRequest<api::HoldTasksReq, api::HoldTasksRsp>("/hold_tasks", req);
}

drogon::Task<std::pair<api::SetTaskRsp, frmwork::Status>> TaskRpc::SetTask(const api::SetTaskReq& req) {
    co_return co_await SendRequest<api::SetTaskReq, api::SetTaskRsp>("/set_task", req);
}

drogon::Task<std::pair<api::CreateTaskRsp, frmwork::Status>> TaskRpc::CreateTask(const api::CreateTaskReq& req) {
    co_return co_await SendRequest<api::CreateTaskReq, api::CreateTaskRsp>("/create_task", req);
}

}
}
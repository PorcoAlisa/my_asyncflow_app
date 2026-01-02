#include "task.h"

namespace async_flow::worker {

drogon::Task<std::pair<api::GetTaskScheduleCfgListRsp, frmwork::Status>> TaskRpc::GetTaskScheduleCfgList(const api::GetTaskScheduleCfgListReq& req) {
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

drogon::Task<frmwork::Status> TaskBase::SetTask() {
    api::SetTaskReq req;
    req.set_task_type(taskData_.task_type());
    req.set_user_id(taskData_.user_id());
    req.set_task_id(taskData_.task_id());
    req.set_task_stage(taskData_.task_stage());
    req.set_status(taskData_.status());
    req.set_crt_retry_num(taskData_.crt_retry_num());
    req.set_max_retry_interval(taskData_.max_retry_interval());
    req.set_schedule_log(taskData_.schedule_log());
    req.set_task_context(taskData_.task_context());

    auto* taskRpc = drogon::app().getPlugin<TaskRpc>();
    if (!taskRpc) {
        LOG_ERROR << "TaskRpc plugin not found! Did you add it to config.json?";
        co_return frmwork::Status::FAIL;
    }
    auto [rsp, status] = co_await taskRpc->SetTask(req);
    if (!status.ok()) {
        LOG_ERROR << "SetTask Failed";
        co_return frmwork::Status::FAIL;
    }
    co_return frmwork::Status::OK;
}

}
#include "create_task.h"
#include "const.h"
#include "task.h"
#include "schedule_pos.h"
#include "schedule_cfg.h"

using namespace async_flow::flowsvr;
using namespace async_flow::frmwork;
using namespace async_flow::db;

Task<async_flow::frmwork::Status> CreateTaskHandler::HandleInput(std::shared_ptr<api::CreateTaskReq>& reqBody) {
    if (reqBody->taskdata().task_type().empty()) {
        LOG_ERROR << "task type is empty";
        co_return Status::FAIL;
    }
    co_return Status::OK;
}

Task<async_flow::frmwork::Status> CreateTaskHandler::HandleProcess(std::shared_ptr<api::CreateTaskReq>& reqBody, api::CreateTaskRsp& rspBody) {
    TaskDao taskDao;
    SchedulePosDao posDao;
    ScheduleCfgDao cfgDao;

    drogon_model::data0::TScheduleCfg schCfg;
    drogon_model::data0::TSchedulePos schPos;
    drogon_model::data0::TLarkTask1 task;

    std::string taskType = reqBody->taskdata().task_type();
    std::string taskTableName = GetTaskTableName(taskType);

    Status status = co_await posDao.GetAsync(taskTableName, schPos);
    if (!status.ok()) co_return Status::FAIL;

    std::string schPosStr = std::to_string(schPos.getValueOfScheduleEndPos());
    status = co_await cfgDao.GetAsync(taskType, schCfg);
    if (!status.ok()) co_return Status::FAIL;

    reqBody->mutable_taskdata()->set_max_retry_num(schCfg.getValueOfMaxRetryNum());
    reqBody->mutable_taskdata()->set_max_retry_interval(schCfg.getValueOfRetryInterval());
    reqBody->mutable_taskdata()->set_order_time(TimestampNow());

    FillDBTaskModel(reqBody->taskdata(), task);
    status = co_await taskDao.CreateAsync(taskType, schPosStr, task);
    if (!status.ok()) co_return status;

    rspBody.set_task_id(task.getValueOfTaskId());
    co_return Status::OK;
}
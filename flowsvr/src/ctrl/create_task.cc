#include "create_task.h"
#include "const.h"
#include "task.h"
#include "schedule_pos.h"
#include "schedule_cfg.h"

namespace async_flow {
namespace flowsvr {

using namespace frmwork;
using namespace db;

Task<Status> CreateTaskHandler::HandleInput(std::shared_ptr<api::CreateTaskReq>& reqBody) {
    if (reqBody->taskdata().task_type().empty()) {
        LOG_ERROR << "task type is empty";
        co_return Status::FAIL;
    }
    co_return Status::OK;
}

Task<std::pair<api::CreateTaskRsp, Status>> CreateTaskHandler::HandleProcess(std::shared_ptr<api::CreateTaskReq>& reqBody) {
    TaskDao taskDao;
    SchedulePosDao posDao;
    ScheduleCfgDao cfgDao;

    drogon_model::data0::TLarkTask1 task;

    std::string taskType = reqBody->taskdata().task_type();
    std::string taskTableName = GetTaskTableName(taskType);

    auto [schPos, status1] = co_await posDao.GetAsync(taskTableName);
    if (!status1.ok()) co_return {{}, Status::FAIL};

    std::string schPosStr = std::to_string(schPos.getValueOfScheduleEndPos());
    auto [schCfg, status2] = co_await cfgDao.GetAsync(taskType);
    if (!status2.ok()) co_return {{}, Status::FAIL};

    reqBody->mutable_taskdata()->set_max_retry_num(schCfg.getValueOfMaxRetryNum());
    reqBody->mutable_taskdata()->set_max_retry_interval(schCfg.getValueOfRetryInterval());
    reqBody->mutable_taskdata()->set_order_time(TimestampNow());

    FillDBTaskModel(reqBody->taskdata(), task);
    Status status3 = co_await taskDao.CreateAsync(taskType, schPosStr, task);
    if (!status3.ok()) co_return {{}, status3};

    api::CreateTaskRsp rspBody;
    rspBody.set_task_id(task.getValueOfTaskId());
    co_return { std::move(rspBody), Status::OK};
}

} // flowsvr
} // async_flow
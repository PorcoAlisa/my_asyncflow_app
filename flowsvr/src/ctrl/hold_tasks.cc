#include "hold_tasks.h"
#include "dao.h"
#include "task.h"
#include "schedule_pos.h"

namespace async_flow {
namespace flowsvr {

using namespace async_flow::frmwork;
using namespace async_flow::db;

const int32_t HoldTasksHandler::MAX_TASK_LIST_LIMIT = 1000;
const int32_t HoldTasksHandler::DEFAULT_TASK_LIST_LIMIT = 1000;

Task<Status> HoldTasksHandler::HandleInput(std::shared_ptr<api::HoldTasksReq>& reqBody) {
    if (reqBody->task_type().empty()) {
        LOG_ERROR << "task type is empty";
        co_return InputInvalid;
    }
    co_return Status::OK;
}

Task<Status> HoldTasksHandler::HandleProcess(std::shared_ptr<api::HoldTasksReq>& reqBody, api::HoldTasksRsp& rspBody) {
    TaskDao taskDao;
    SchedulePosDao posDao;

    int limit = reqBody->limit();
    if (limit > MAX_TASK_LIST_LIMIT) {
        limit = MAX_TASK_LIST_LIMIT;
    }

    if (limit == 0) {
        limit = DEFAULT_TASK_LIST_LIMIT;
    }
    std::string taskType = reqBody->task_type();
    std::string taskTableName = GetTaskTableName(taskType);

    auto [schPos, status] = co_await posDao.GetAsync(taskTableName);
    if (!status.ok()) {
        co_return Status::FAIL;
    }
    std::string beginSchPos = std::to_string(schPos.getValueOfScheduleBeginPos());
    std::vector<drogon_model::data0::TLarkTask1> vecTasks;
    status = co_await taskDao.GetTaskListAsync(taskType, beginSchPos, PENDING, limit, vecTasks);
    if (!status.ok()) {
        co_return Status::FAIL;
    }

    std::vector<std::string> taskIDs;
    for (auto& task : vecTasks) {
        if (task.getValueOfCrtRetryNum() != 0 && task.getValueOfMaxRetryInterval() != 0 
            && task.getValueOfOrderTime() > TimestampNow()) {
            continue;
        }
        taskIDs.push_back(task.getValueOfTaskId());
        api::TaskData* taskData = rspBody.add_task_list();
        FillPBTaskModel(task, *taskData);
    }

    status = co_await taskDao.BatchSetStatusAsync(taskIDs, PROCESSING);
    if (!status.ok()) {
        LOG_INFO << "BatchSetStatus: " << status.error_code();
        co_return status;
    }

    co_return Status::OK;
}

} // flowsvr
} // async_flow
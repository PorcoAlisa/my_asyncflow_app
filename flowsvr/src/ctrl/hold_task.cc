#include "hold_task.h"

using namespace async_flow::frmwork;


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
    TSchedulePos schPos;

    int limit = reqBody->limit();
    if (limit > MAX_TASK_LIST_LIMIT) {
        limit = MAX_TASK_LIST_LIMIT;
    }

    if (limit == 0) {
        limit = DEFAULT_TASK_LIST_LIMIT;
    }
    std::string taskType = reqBody->task_type();
    std::string taskTableName = GetTaskTableName(taskType);

    Status status = co_await posDao.GetAsync(taskTableName, schPos);
    if (!status.ok()) {
        co_return Status::FAIL;
    }
    std::string beginSchPos = std::to_string(schPos.getValueOfScheduleBeginPos());
    co_return Status::OK;
}
#include "hold_tasks.h"
#include "dao.h"
#include "task.h"
#include "schedule_pos.h"

namespace async_flow::flowsvr {

using namespace async_flow::frmwork;
using namespace async_flow::db;

const int32_t HoldTasksHandler::MAX_TASK_LIST_LIMIT = 1000;
const int32_t HoldTasksHandler::DEFAULT_TASK_LIST_LIMIT = 1000;

drogon::Task<Status> HoldTasksHandler::HandleInput(std::shared_ptr<api::HoldTasksReq>& reqBody) {
    if (reqBody->task_type().empty()) {
        LOG_ERROR << "task type is empty";
        co_return InputInvalid;
    }
    co_return Status::OK;
}

drogon::Task<std::pair<api::HoldTasksRsp, Status>> HoldTasksHandler::HandleProcess(std::shared_ptr<api::HoldTasksReq>& reqBody) {
    TaskDao taskDao;
    SchedulePosDao posDao;

    int limit = reqBody->limit();
    if (limit > MAX_TASK_LIST_LIMIT) limit = MAX_TASK_LIST_LIMIT;
    if (limit == 0) limit = DEFAULT_TASK_LIST_LIMIT;

    std::string taskType = reqBody->task_type();
    std::string taskTableName = GetTaskTableName(taskType);

    auto [schPos, status] = co_await posDao.GetAsync(taskTableName);
    if (!status.ok()) {
        co_return {{}, Status::FAIL};
    }

    auto dbClient = drogon::app().getDbClient("porcodb");
    if (!dbClient) {
        LOG_ERROR << "dbClient 'porcodb' not found!";
        co_return {{}, Status::FAIL};
    }
    std::shared_ptr<drogon::orm::Transaction> transPtr;
    try {
        transPtr = co_await dbClient->newTransactionCoro();
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to begin transaction: " << e.what();
        co_return {{}, Status::FAIL};
    }

    std::string beginSchPos = std::to_string(schPos.getValueOfScheduleBeginPos());
    std::string tableName = taskTableName + "_" + beginSchPos;
    auto [vecTasks, status1] = co_await TaskDao::GetTaskListWithTxAsync(transPtr, taskType, TASK_PENDING, beginSchPos, limit);
    if (!status1.ok()) { co_return {{}, status1}; }

    api::HoldTasksRsp rspBody;
    std::vector<std::int32_t> ids;
    for (auto& task : vecTasks) {
        ids.push_back(task.getValueOfId());
        api::TaskData* taskData = rspBody.add_task_list();
        FillPBTaskModel(task, *taskData);
    }

    status = co_await TaskDao::BatchSetStatusWithTxAsync(transPtr, ids, TASK_PROCESSING, taskTableName);
    if (!status.ok()) {
        LOG_INFO << "BatchSetStatus: " << status.error_code();
        co_return {rspBody, status};
    }

    try {
        co_await transPtr->execSqlCoro("COMMIT");
    } catch (const std::exception& e) {
        LOG_ERROR << "Transaction commit failed: " << e.what();
        co_return {{}, Status::FAIL};
    }

    co_return {rspBody, Status::OK};
}

}

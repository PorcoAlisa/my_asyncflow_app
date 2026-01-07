#include <format>
#include "task.h"
#include "comm.h"
#include "const.h"

namespace async_flow::db {

using namespace frmwork;

std::string GetTableName(std::string_view taskType, std::string_view pos) {
    return std::format("t_{}_task_{}", taskType, pos);
}

std::string GenTaskID(std::string_view taskType, std::string_view pos) {
    std::string filterTaskType = ReplaceString(taskType, "_", "-");
    return std::format("{}_{}_{}", generate_uuid_v4(), filterTaskType, pos);
}

std::pair<std::string_view, std::string_view> GetTablePosFromTaskID(std::string_view taskID) {
    std::vector<std::string_view> splitStr = StringSplit(taskID, '_');
    if (splitStr.size() < 3) {
        LOG_ERROR << "task id invalid, taskID = " << taskID;
        return {std::string_view{}, std::string_view{}};
    }
    return {splitStr[1], splitStr[2]};
}

drogon::Task<Status> TaskDao::CreateAsync(const std::string& taskType, const std::string& pos, drogon_model::data0::TLarkTask1& task) const {
    try {
        std::string tableName = GetTableName(taskType, pos);
        if (task.getValueOfTaskId() == "") {
            task.setTaskId(GenTaskID(taskType, pos));
        }
        task.setOrderTime(TimestampNow());
        task.setTaskType(taskType);
        auto result = co_await clientPtr_->execSqlCoro(
            "insert into " + tableName +
            " (user_id, task_id, task_type, task_stage, status, priority, crt_retry_num, max_retry_num, max_retry_interval, schedule_log, task_context, order_time) "
            "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            task.getValueOfUserId(), task.getValueOfTaskId(), task.getValueOfTaskType(), task.getValueOfTaskStage(), task.getValueOfStatus(), task.getValueOfPriority(),
            task.getValueOfCrtRetryNum(), task.getValueOfMaxRetryNum(), task.getValueOfMaxRetryInterval(), task.getValueOfScheduleLog(), task.getValueOfTaskContext(), task.getValueOfOrderTime()
        );

        LOG_INFO << "create_task " << result.affectedRows() << "rows affected";
        if (result.affectedRows() > 0) {
            co_return Status::OK;
        }
        co_return Status::FAIL;
    } catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        co_return Status::FAIL;
    }
}


drogon::Task<Status> TaskDao::GetTaskListAsync(const std::string& taskType, const std::string& pos, const TaskStatus& status, int limit, std::vector<drogon_model::data0::TLarkTask1>& tasklist) const {
    std::string tableName = GetTableName(taskType, pos);
    try {
        auto result = co_await clientPtr_->execSqlCoro(
            "select * from " + tableName + " where task_type = ? and status = ? order by order_time desc limit " + std::to_string(limit), taskType, status);
        if (result.empty()) {
            co_return ResourceNotFound;
        }
        LOG_INFO << "task list size: " << result.size();
        tasklist.reserve(result.size());
        for (const auto& row : result) {
            tasklist.emplace_back(row);
        }
    } catch (const drogon::orm::DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        co_return DBExecErr;
    }
    co_return Status::OK;
}

drogon::Task<Status> TaskDao::BatchSetStatusAsync(const std::vector<std::string>& taskIDList, const TaskStatus& status) const {
    if (taskIDList.empty()) {
        co_return Status::OK;
    }

    auto [taskType, pos] = GetTablePosFromTaskID(taskIDList[0]);
    std::string tableName = GetTableName(taskType, pos);

    std::vector<std::string> quoteTaskIDList;
    for (const std::string& taskID : taskIDList) {
        quoteTaskIDList.push_back("'" + taskID + "'");
    }
    std::string taskInCond = StringJoin(quoteTaskIDList, ',');

    try {
        auto result = co_await clientPtr_->execSqlCoro("update " + tableName + " set status = ? where task_id in ( " + taskInCond + " )", status);
    } catch (const drogon::orm::DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        co_return DBExecErr;
    }
    co_return Status::OK;
}

drogon::Task<Status> TaskDao::SaveAsync(const drogon_model::data0::TLarkTask1& task) const {
    auto [taskType, pos] = GetTablePosFromTaskID(task.getValueOfTaskId());
    if (taskType.empty() || pos.empty()) co_return InputInvalid;
    std::string tableName = GetTableName(taskType, pos);

    try {
        auto result = co_await clientPtr_->execSqlCoro(
            "update " + tableName + " set user_id = ?, task_type = ?, task_stage = ?, status = ?, priority = ?, crt_retry_num = ?, "
            "max_retry_num = ?, max_retry_interval = ?, schedule_log = ?, task_context = ?, order_time = ? "
            "where task_id = ?",
            task.getValueOfUserId(), task.getValueOfTaskType(), task.getValueOfTaskStage(), task.getValueOfStatus(),
            task.getValueOfPriority(), task.getValueOfCrtRetryNum(), task.getValueOfMaxRetryNum(), task.getValueOfMaxRetryInterval(),
            task.getValueOfScheduleLog(), task.getValueOfTaskContext(), task.getValueOfOrderTime(), task.getValueOfTaskId()
        );
        LOG_INFO << "update task " << result.affectedRows() << " rows affected";
    } catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        co_return DBExecErr;
    }
    co_return Status::OK;
}

drogon::Task<std::pair<std::vector<drogon_model::data0::TLarkTask1>, Status>>
TaskDao::GetTaskListWithTxAsync(const std::shared_ptr<drogon::orm::DbClient>& clientPtr,
    const std::string& taskType, const TaskStatus& status, const std::string& pos, int limit)
{
    std::string tableName = GetTableName(taskType, pos);
    std::string sql = std::format(R"(
        SELECT * FROM {}
        WHERE status = ?
          AND order_time <= ?
        ORDER BY order_time DESC
        LIMIT ?
        FOR UPDATE SKIP LOCKED
    )", tableName);

    std::vector<drogon_model::data0::TLarkTask1> tasklist;
    try {
        auto result = co_await clientPtr->execSqlCoro(sql, status, TimestampNow(), limit);
        if (result.empty()) co_return {{}, ResourceNotFound};

        LOG_INFO << "Fetched & Locked tasks size: " << result.size();
        tasklist.reserve(result.size());
        for (const auto& row : result) {
            tasklist.emplace_back(row);
        }
    } catch (const drogon::orm::DrogonDbException& e) {
        LOG_ERROR << "FetchAndLock Error: " << e.base().what();
        co_return {{}, DBExecErr};
    }
    co_return {tasklist, Status::OK};
}

drogon::Task<Status> TaskDao::BatchSetStatusWithTxAsync(const std::shared_ptr<drogon::orm::DbClient>& clientPtr,
        const std::vector<int32_t>& ids, int newStatus, std::string_view tableName)
{
    if (ids.empty()) co_return Status::OK;

    std::string idInCond;
    idInCond.reserve(ids.size() * 10);
    for (size_t i = 0; i < ids.size(); ++i) {
        idInCond += std::to_string(ids[i]);
        if (i < ids.size() - 1) idInCond += ",";
    }
    try {
        std::string sql = std::format("UPDATE {} SET status = ? WHERE id IN ({})", tableName, idInCond);
        auto result = co_await clientPtr->execSqlCoro(sql, newStatus);
    } catch (const drogon::orm::DrogonDbException& e) {
        co_return DBExecErr;
    }
    co_return Status::OK;
}

}
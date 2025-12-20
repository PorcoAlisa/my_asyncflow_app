#include <format>
#include "task.h"
#include "comm.h"
#include "const.h"

using namespace async_flow::db;
using namespace async_flow::frmwork;
using namespace drogon::orm;
using TLarkTask1 = drogon_model::data0::TLarkTask1;

namespace async_flow {
namespace db {

std::string GetTableName(std::string_view taskType, std::string_view pos) {
    // return Sprintf("t_%s_task_%s", taskType.c_str(), pos.c_str());
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

drogon::Task<Status> TaskDao::CreateAsync(const std::string& taskType, const std::string& pos, TLarkTask1& task) {
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
        } else {
            co_return Status::FAIL;
        }
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        co_return Status::FAIL;
    }
    co_return Status::OK;
}

void row2Task(drogon_model::data0::TLarkTask1& task, const drogon::orm::Row& r) {
    task.setId(r["id"].as<int32_t>());
    task.setUserId(r["user_id"].as<std::string>());
    task.setTaskId(r["task_id"].as<std::string>());
    task.setTaskType(r["task_type"].as<std::string>());
    task.setTaskStage(r["task_stage"].as<std::string>());
    task.setStatus(r["status"].as<uint8_t>());
    task.setPriority(r["priority"].as<int32_t>());
    task.setCrtRetryNum(r["crt_retry_num"].as<int32_t>());
    task.setMaxRetryNum(r["max_retry_num"].as<int32_t>());
    task.setMaxRetryInterval(r["max_retry_interval"].as<int32_t>());
    task.setScheduleLog(r["schedule_log"].as<std::string>());
    task.setTaskContext(r["task_context"].as<std::string>());
    task.setOrderTime(r["order_time"].as<int32_t>());
    task.setCreateTime(trantor::Date::fromDbStringLocal(r["create_time"].as<std::string>()));
    task.setModifyTime(trantor::Date::fromDbStringLocal(r["modify_time"].as<std::string>()));
}

drogon::Task<async_flow::frmwork::Status> TaskDao::GetTaskListAsync(const std::string& taskType, std::string& pos, const TaskStatus& status, int limit, std::vector<drogon_model::data0::TLarkTask1>& tasklist) {
    try {
        std::string tableName = GetTableName(taskType, pos);
        auto result = co_await clientPtr_->execSqlCoro(
            "select * from " + tableName + " where task_type = ? and status = ? order by order_time desc limit " + std::to_string(limit), taskType, status);
        if (result.empty()) {
            co_return ResourceNotFound;
        }
        LOG_INFO << "task list size: " << result.size();
        tasklist.reserve(result.size());
        for (const auto& row : result) {
            tasklist.emplace_back();
            row2Task(tasklist.back(), row);
        }
    } catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        co_return DBExecErr;
    }
    co_return Status::OK;
}



drogon::Task<async_flow::frmwork::Status> TaskDao::BatchSetStatusAsync(const std::vector<std::string>& taskIDList, const TaskStatus& status) {
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
    } catch (const DrogonDbException& e) {
        LOG_ERROR << "error:" << e.base().what();
        co_return DBExecErr;
    }
    co_return Status::OK;
}


}; // namespace db
}; // namespace async_flow
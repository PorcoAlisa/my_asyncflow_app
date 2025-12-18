#include "task.h"
#include "comm.h"

using namespace async_flow::db;
using namespace async_flow::frmwork;
using namespace drogon::orm;
using TLarkTask1 = drogon_model::data0::TLarkTask1;

namespace async_flow {
namespace db {


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
    } catch (const drogon::orm::DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        co_return Status::FAIL;
    }
    co_return Status::OK;
}


std::string TaskDao::GetTableName(const std::string& taskType, const std::string& pos) {
    return Sprintf("t_%s_task_%s", taskType.c_str(), pos.c_str());
}

std::string TaskDao::GenTaskID(const std::string& taskType, const std::string& pos) {
    std::string filterTaskType(taskType);
    ReplaceString(filterTaskType, "_", "-");
    return Sprintf("%s_%s_%s", generate_uuid_v4().c_str(), filterTaskType.c_str(), pos.c_str());
}

}; // namespace db
}; // namespace async_flow
#pragma once

#include "TLarkTask1.h"
#include "dao.h"
#include "status.h"
#include "comm.h"

namespace async_flow {
namespace db {

enum TaskStatus { PENDING, PROCESSING, SUCC, FAILED };

class TaskDao : public Dao {
public:
    TaskDao() {}

    drogon::Task<async_flow::frmwork::Status> CreateAsync(const std::string& taskType, const std::string& pos, drogon_model::data0::TLarkTask1& task);
    drogon::Task<async_flow::frmwork::Status> GetTaskListAsync(const std::string& taskType, std::string& pos, const TaskStatus& status, int limit, std::vector<drogon_model::data0::TLarkTask1>& tasklist);
    drogon::Task<async_flow::frmwork::Status> BatchSetStatusAsync(const std::vector<std::string>& taskIDList, const TaskStatus& status);
    drogon::Task<async_flow::frmwork::Status> SaveAsync(const drogon_model::data0::TLarkTask1& task);
private:

};

}; // namespace db
}; // namespace async_flow


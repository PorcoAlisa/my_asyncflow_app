#pragma once

#include "TLarkTask1.h"
#include "dao.h"
#include "status.h"
#include "comm.h"

namespace async_flow {
namespace db {

class TaskDao : public Dao {
public:
    TaskDao() {}

    drogon::Task<frmwork::Status> CreateAsync(const std::string& taskType, const std::string& pos, drogon_model::data0::TLarkTask1& task);
    drogon::Task<frmwork::Status> GetTaskListAsync(const std::string& taskType, std::string& pos, const frmwork::TaskStatus& status, int limit, std::vector<drogon_model::data0::TLarkTask1>& tasklist);
    drogon::Task<frmwork::Status> BatchSetStatusAsync(const std::vector<std::string>& taskIDList, const frmwork::TaskStatus& status);
    drogon::Task<frmwork::Status> SaveAsync(const drogon_model::data0::TLarkTask1& task);
private:

};

} // namespace db
} // namespace async_flow


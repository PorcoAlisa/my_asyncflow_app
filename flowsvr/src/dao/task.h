#pragma once

#include "TLarkTask1.h"
#include "dao.h"
#include "status.h"
#include "comm.h"

namespace async_flow {
namespace db {

enum TaskStatus { Pending, Processing, Succ, Failed };

class TaskDao : public Dao {
public:
    TaskDao() {}
    drogon::Task<async_flow::frmwork::Status> CreateAsync(const std::string& taskType, const std::string& pos, drogon_model::data0::TLarkTask1& task);
    std::string GetTableName(const std::string& taskType, const std::string& pos);
    std::string GenTaskID(const std::string& taskType, const std::string& pos);
private:
    
};

}; // namespace db
}; // namespace async_flow


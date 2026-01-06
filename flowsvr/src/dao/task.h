#pragma once

#include "TLarkTask1.h"
#include "dao.h"
#include "status.h"
#include "comm.h"

namespace async_flow::db {

class TaskDao : public Dao {
    template <class T>
    using Task = drogon::Task<T>;
    using Status = frmwork::Status;
    using DbClient = drogon::orm::DbClient;
    using TLarkTask1 = drogon_model::data0::TLarkTask1;

public:
    TaskDao() = default;
    Task<Status> CreateAsync(const std::string& taskType, const std::string& pos, TLarkTask1& task) const;
    Task<Status> GetTaskListAsync(const std::string& taskType, const std::string& pos,
        const frmwork::TaskStatus& status, int limit, std::vector<TLarkTask1>& tasklist) const;
    [[nodiscard]] Task<Status> BatchSetStatusAsync(
        const std::vector<std::string>& taskIDList, const frmwork::TaskStatus& status) const;
    [[nodiscard]] Task<Status> SaveAsync(const TLarkTask1& task) const;
    static Task<std::pair<std::vector<TLarkTask1>, Status>> GetTaskListWithTxAsync(
        const std::shared_ptr<DbClient>& clientPtr, const std::string& taskType, const frmwork::TaskStatus& status,
        const std::string& pos, int limit);
    static Task<Status> BatchSetStatusWithTxAsync(const std::shared_ptr<DbClient>& clientPtr,
        const std::vector<int32_t>& taskIDs, int newStatus, std::string_view tableName);
};

}

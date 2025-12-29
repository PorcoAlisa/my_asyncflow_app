#include "schedule_cfg.h"
#include "const.h"

namespace async_flow {
namespace db {

using namespace frmwork;
using namespace drogon::orm;

using TScheduleCfg = drogon_model::data0::TScheduleCfg;
// using Status = async_flow::frmwork::Status;

Status ScheduleCfgDao::Create(TScheduleCfg& cfg) {
    try {
        Mapper<TScheduleCfg> mp(clientPtr_);
        mp.insert(cfg);
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error: " << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

Status ScheduleCfgDao::Save(TScheduleCfg& cfg) {
    try {
        Mapper<TScheduleCfg> mp(clientPtr_);
        mp.update(cfg);
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error: " << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

drogon::Task<std::pair<TScheduleCfg, Status>> ScheduleCfgDao::GetAsync(const std::string& taskType) {
    try {
        CoroMapper<TScheduleCfg> mp(clientPtr_);
        auto tasks = co_await mp.limit(1).findBy(Criteria(TScheduleCfg::Cols::_task_type, CompareOperator::EQ, taskType));
        if (tasks.empty()) {
            LOG_FATAL << "error tasks size = 0";
            co_return {{}, ResourceNotFound};
        } else {
            co_return {std::move(tasks[0]), Status::OK};
        }
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error: " << e.base().what();
        co_return {{}, DBExecErr};
    }
}

drogon::Task<std::pair<std::vector<TScheduleCfg>, Status>> ScheduleCfgDao::GetListAsync() {
    try {
        CoroMapper<TScheduleCfg> mp(clientPtr_);
        std::vector<TScheduleCfg> cfgs = co_await mp.findAll();
        co_return {std::move(cfgs), Status::OK};
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error: " << e.base().what();
        co_return {{}, DBExecErr};
    }
}


} // namespace db
} // namespace async_flow
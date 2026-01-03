#include "schedule_cfg.h"
#include "const.h"

namespace async_flow::db {

using namespace frmwork;
using namespace drogon::orm;

Status ScheduleCfgDao::Create(drogon_model::data0::TScheduleCfg& cfg) const {
    try {
        Mapper<drogon_model::data0::TScheduleCfg> mp(clientPtr_);
        mp.insert(cfg);
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error: " << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

Status ScheduleCfgDao::Save(const drogon_model::data0::TScheduleCfg& cfg) const {
    try {
        Mapper<drogon_model::data0::TScheduleCfg> mp(clientPtr_);
        mp.update(cfg);
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error: " << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

drogon::Task<std::pair<drogon_model::data0::TScheduleCfg, Status>> ScheduleCfgDao::GetAsync(const std::string& taskType) const {
    try {
        CoroMapper<drogon_model::data0::TScheduleCfg> mp(clientPtr_);
        if (auto tasks = co_await mp.limit(1).findBy(Criteria(drogon_model::data0::TScheduleCfg::Cols::_task_type, CompareOperator::EQ, taskType)); tasks.empty()) {
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

drogon::Task<std::pair<std::vector<drogon_model::data0::TScheduleCfg>, Status>> ScheduleCfgDao::GetListAsync() const {
    try {
        CoroMapper<drogon_model::data0::TScheduleCfg> mp(clientPtr_);
        std::vector<drogon_model::data0::TScheduleCfg> cfgs = co_await mp.findAll();
        co_return {std::move(cfgs), Status::OK};
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error: " << e.base().what();
        co_return {{}, DBExecErr};
    }
}

}

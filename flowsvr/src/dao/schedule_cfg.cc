#include "schedule_cfg.h"
#include "const.h"

using namespace async_flow::db;
using namespace async_flow::frmwork;
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

drogon::Task<Status> ScheduleCfgDao::GetAsync(const std::string& taskType, TScheduleCfg& pos) {
    try {
        CoroMapper<TScheduleCfg> mp(clientPtr_);
        auto tasks = co_await mp.limit(1).findBy(Criteria(TScheduleCfg::Cols::_task_type, CompareOperator::EQ, taskType));
        if (tasks.empty()) {
            LOG_FATAL << "error tasks size = 0";
            co_return ResourceNotFound;
        } else {
            pos = tasks[0];
        }
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error: " << e.base().what();
        co_return DBExecErr;
    }
    co_return Status::OK;
}

Status ScheduleCfgDao::GetList(std::vector<TScheduleCfg>& cfgs) {
    try {
        Mapper<TScheduleCfg> mp(clientPtr_);
        cfgs = mp.findAll();
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error: " << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}
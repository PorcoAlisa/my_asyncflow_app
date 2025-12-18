#include "schedule_pos.h"
#include "const.h"

using namespace async_flow::db;
using namespace async_flow::frmwork;
using namespace drogon::orm;

using TSchedulePos = drogon_model::data0::TSchedulePos;

Status SchedulePosDao::Create(TSchedulePos& pos) {
    try {
        Mapper<TSchedulePos> mp(clientPtr_);
        mp.insert(pos);
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

Status SchedulePosDao::Save(TSchedulePos& pos) {
    try {
        Mapper<TSchedulePos> mp(clientPtr_);
        mp.update(pos);
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        return DBExecErr;
    }
    return Status::OK;
}

drogon::Task<async_flow::frmwork::Status> SchedulePosDao::GetAsync(const std::string& taskSetName, TSchedulePos& pos) {
    try {
        CoroMapper<TSchedulePos> mp(clientPtr_);
        auto tasks = co_await mp.limit(1).findBy(Criteria(TSchedulePos::Cols::_task_type, CompareOperator::EQ, taskSetName));
        if (tasks.empty()) {
            co_return ResourceNotFound;
        } else {
            pos = tasks[0];
        }
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
        co_return DBExecErr;
    }
    co_return Status::OK;
}

drogon::Task<Status> SchedulePosDao::GetRandomSchedulePosAsync(const std::string& taskSetName, int& pos) {
    TSchedulePos taskPos;
    Status status = co_await GetAsync(taskSetName, taskPos);
    if (!status.ok()) {
        co_return status;
    }
    int base = taskPos.getValueOfScheduleEndPos() - taskPos.getValueOfScheduleBeginPos() + 1;
    pos = (std::rand() % base) + taskPos.getValueOfScheduleBeginPos();
    co_return Status::OK;
}

drogon::Task<Status> SchedulePosDao::GetBeginSchedulePosAsync(const std::string& taskSetName, int& pos) {
    TSchedulePos taskPos;
    Status status = co_await GetAsync(taskSetName, taskPos);
    if (!status.ok()) {
        co_return status;
    }
    pos = taskPos.getValueOfScheduleBeginPos();
    co_return Status::OK;
}

Status SchedulePosDao::GetNextPos(const std::string& pos, std::string& nextPos) {
    int posInt = std::stoi(nextPos);
    nextPos = std::to_string(posInt + 1);
    return Status::OK;
}

Status SchedulePosDao::GetPosList(std::vector<TSchedulePos>& vec) {
    try {
        Mapper<TSchedulePos> mp(clientPtr_);
        vec = mp.findAll();
    } catch (const DrogonDbException& e) {
        LOG_FATAL << "error:" << e.base().what();
    }
    return Status::OK;
}
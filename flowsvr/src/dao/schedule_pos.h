#pragma once

#include "dao.h"
#include "status.h"
#include "TSchedulePos.h"

namespace async_flow {
namespace db {

// using namespace drogon_model::data0;
// using namespace async_flow::frmwork;

using TSchedulePos = drogon_model::data0::TSchedulePos;

class SchedulePosDao : public Dao {
public:
    SchedulePosDao() {}
    async_flow::frmwork::Status Create(drogon_model::data0::TSchedulePos& pos);
    async_flow::frmwork::Status Save(drogon_model::data0::TSchedulePos& pos);
    drogon::Task<async_flow::frmwork::Status> GetAsync(const std::string& taskSetName, drogon_model::data0::TSchedulePos& pos);
    drogon::Task<async_flow::frmwork::Status> GetRandomSchedulePosAsync(const std::string& taskSetName, int& pos);
    drogon::Task<async_flow::frmwork::Status> GetBeginSchedulePosAsync(const std::string& taskSetName, int& pos);
    async_flow::frmwork::Status GetNextPos(const std::string& pos, std::string& nextPos);
    async_flow::frmwork::Status GetPosList(std::vector<drogon_model::data0::TSchedulePos>& vec);
};

}; // namespace db
}; // namespace async_flow
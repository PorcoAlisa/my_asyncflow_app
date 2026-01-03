#pragma once

#include "dao.h"
#include "status.h"
#include "TSchedulePos.h"

namespace async_flow {
namespace db {

using TSchedulePos = drogon_model::data0::TSchedulePos;

class SchedulePosDao : public Dao {
public:
    SchedulePosDao() {}
    frmwork::Status Create(drogon_model::data0::TSchedulePos& pos);
    frmwork::Status Save(drogon_model::data0::TSchedulePos& pos);
    drogon::Task<std::pair<drogon_model::data0::TSchedulePos, frmwork::Status>> GetAsync(const std::string& taskSetName);
    drogon::Task<frmwork::Status> GetRandomSchedulePosAsync(const std::string& taskSetName, int& pos);
    drogon::Task<frmwork::Status> GetBeginSchedulePosAsync(const std::string& taskSetName, int& pos);
    frmwork::Status GetNextPos(const std::string& pos, std::string& nextPos);
    frmwork::Status GetPosList(std::vector<drogon_model::data0::TSchedulePos>& vec);
};

} // namespace db
} // namespace async_flow
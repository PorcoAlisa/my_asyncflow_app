#pragma once

#include "dao.h"
#include "status.h"
#include "TScheduleCfg.h"

namespace async_flow {
namespace db {

class ScheduleCfgDao : public Dao {
public:
    ScheduleCfgDao() {}
    frmwork::Status Create(drogon_model::data0::TScheduleCfg& cfg);
    frmwork::Status Save(drogon_model::data0::TScheduleCfg& cfg);
    drogon::Task<std::pair<drogon_model::data0::TScheduleCfg, frmwork::Status>> GetAsync(const std::string& taskType);
    drogon::Task<std::pair<std::vector<drogon_model::data0::TScheduleCfg>, frmwork::Status>> GetListAsync();
};

} // namespace db
} // namespace async_flow
#pragma once

#include "dao.h"
#include "status.h"
#include "TScheduleCfg.h"

namespace async_flow {
namespace db {

class ScheduleCfgDao : public Dao {
public:
    ScheduleCfgDao() {}
    frmwork::Status Create(drogon_model::data0::TScheduleCfg& cfg) const;
    frmwork::Status Save(const drogon_model::data0::TScheduleCfg& cfg) const;
    drogon::Task<std::pair<drogon_model::data0::TScheduleCfg, frmwork::Status>> GetAsync(const std::string& taskType) const;
    drogon::Task<std::pair<std::vector<drogon_model::data0::TScheduleCfg>, frmwork::Status>> GetListAsync() const;
};

} // namespace db
} // namespace async_flow
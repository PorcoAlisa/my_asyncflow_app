#pragma once

#include "dao.h"
#include "status.h"
#include "TScheduleCfg.h"

namespace async_flow {
namespace db {

class ScheduleCfgDao : public Dao {
public:
    ScheduleCfgDao() {}
    async_flow::frmwork::Status Create(drogon_model::data0::TScheduleCfg& cfg);
    async_flow::frmwork::Status Save(drogon_model::data0::TScheduleCfg& cfg);
    drogon::Task<async_flow::frmwork::Status> GetAsync(const std::string& taskType, drogon_model::data0::TScheduleCfg& cfg);
    async_flow::frmwork::Status GetList(std::vector<drogon_model::data0::TScheduleCfg>& cfg);
};

}; // namespace db
}; // namespace async_flow
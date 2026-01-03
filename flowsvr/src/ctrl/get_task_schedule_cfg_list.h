#pragma once

#include "api.pb.h"
#include "ctrl.h"

namespace async_flow::flowsvr {

class GetTaskScheduleCfgListHandler : public Handler<api::GetTaskScheduleCfgListReq, api::GetTaskScheduleCfgListRsp> {
public:
    drogon::Task<frmwork::Status> HandleInput(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody) override;
    drogon::Task<std::pair<api::GetTaskScheduleCfgListRsp, frmwork::Status>> HandleProcess(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody) override;
};

}

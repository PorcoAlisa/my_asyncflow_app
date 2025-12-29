#pragma once

#include "api.pb.h"
#include "ctrl.h"

namespace async_flow {
namespace flowsvr {

class GetTaskScheduleCfgListHandler : public Handler<api::GetTaskScheduleCfgListReq, api::GetTaskScheduleCfgListRsp> {
public:
    virtual Task<async_flow::frmwork::Status> HandleInput(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody) override;
    virtual Task<std::pair<api::GetTaskScheduleCfgListRsp, async_flow::frmwork::Status>> HandleProcess(std::shared_ptr<api::GetTaskScheduleCfgListReq>& reqBody) override;
};

}
}
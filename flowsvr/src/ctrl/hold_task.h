#pragma once

#include "ctrl.h"
#include "api.pb.h"
#include <drogon/drogon.h>

namespace async_flow {
namespace flowsvr {

class HoldTaskHandler : public Handler<api::HoldTasksReq, api::HoldTasksRsp> {
public:
    virtual Task<frmwork::Status> HandleInput(std::shared_ptr<api::HoldTasksReq>& reqBody) override;
    virtual Task<frmwork::Status> HandleProcess(std::shared_ptr<api::HoldTasksReq>& reqBody, api::HoldTasksRsp& rspBody) override;
};

} // flowsvr
} // async_flow
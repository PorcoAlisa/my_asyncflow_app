#pragma once

#include "ctrl.h"
#include "api.pb.h"
#include <drogon/drogon.h>

namespace async_flow {
namespace flowsvr {

class CreateTaskHandler : public Handler<api::CreateTaskReq, api::CreateTaskRsp> {
public:
    virtual Task<async_flow::frmwork::Status> HandleInput(std::shared_ptr<api::CreateTaskReq>& reqBody) override;
    virtual Task<async_flow::frmwork::Status> HandleProcess(std::shared_ptr<api::CreateTaskReq>& reqBody, api::CreateTaskRsp& rspBody) override;
};

} // flowsvr
} // async_flow
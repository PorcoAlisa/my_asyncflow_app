#pragma once

#include "api.pb.h"
#include "ctrl.h"
#include <drogon/drogon.h>

namespace async_flow {
namespace flowsvr {

class SetTaskHandler : public Handler<api::SetTaskReq, api::SetTaskRsp> {
public:
    virtual Task<async_flow::frmwork::Status> HandleInput(std::shared_ptr<api::SetTaskReq>& reqBody) override;
    virtual Task<async_flow::frmwork::Status> HandleProcess(std::shared_ptr<api::SetTaskReq>& reqBody, api::SetTaskRsp& rspBody) override;

};

}
}
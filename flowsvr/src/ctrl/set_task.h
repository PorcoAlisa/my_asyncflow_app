#pragma once

#include "api.pb.h"
#include "ctrl.h"
#include <drogon/drogon.h>

namespace async_flow::flowsvr {

class SetTaskHandler : public Handler<api::SetTaskReq, api::SetTaskRsp> {
public:
    drogon::Task<frmwork::Status> HandleInput(std::shared_ptr<api::SetTaskReq>& reqBody) override;
    drogon::Task<std::pair<api::SetTaskRsp, frmwork::Status>> HandleProcess(std::shared_ptr<api::SetTaskReq>& reqBody) override;
};

}

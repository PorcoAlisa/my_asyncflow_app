#pragma once

#include "ctrl.h"
#include "api.pb.h"
#include <drogon/drogon.h>

namespace async_flow::flowsvr {

class CreateTaskHandler : public Handler<api::CreateTaskReq, api::CreateTaskRsp> {
public:
    drogon::Task<frmwork::Status> HandleInput(std::shared_ptr<api::CreateTaskReq>& reqBody) override;
    drogon::Task<std::pair<api::CreateTaskRsp, frmwork::Status>> HandleProcess(std::shared_ptr<api::CreateTaskReq>& reqBody) override;
};

}

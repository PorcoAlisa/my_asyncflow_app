#pragma once

#include "ctrl.h"
#include "api.pb.h"
#include <drogon/drogon.h>

namespace async_flow::flowsvr {

class HoldTasksHandler : public Handler<api::HoldTasksReq, api::HoldTasksRsp> {
public:
    drogon::Task<frmwork::Status> HandleInput(std::shared_ptr<api::HoldTasksReq>& reqBody) override;
    drogon::Task<std::pair<api::HoldTasksRsp, frmwork::Status>> HandleProcess(std::shared_ptr<api::HoldTasksReq>& reqBody) override;

    static const int32_t MAX_TASK_LIST_LIMIT;
    static const int32_t DEFAULT_TASK_LIST_LIMIT;
};

}

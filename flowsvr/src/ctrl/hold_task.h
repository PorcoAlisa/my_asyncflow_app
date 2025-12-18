#pragma once

#include "ctrl.h"
#include "api.pb.h"
#include <drogon/drogon.h>

namespace async_flow {
namespace flowsvr {

class HoldTasksHandler : public Handler<api::HoldTasksReq, api::HoldTasksRsp> {
public:
    virtual Task<frmwork::Status> HandleInput(std::shared_ptr<api::HoldTasksReq>& reqBody) override;
    virtual Task<frmwork::Status> HandleProcess(std::shared_ptr<api::HoldTasksReq>& reqBody, api::HoldTasksRsp& rspBody) override;
    
    static const int32_t MAX_TASK_LIST_LIMIT;
    static const int32_t DEFAULT_TASK_LIST_LIMIT;
};

} // flowsvr
} // async_flow
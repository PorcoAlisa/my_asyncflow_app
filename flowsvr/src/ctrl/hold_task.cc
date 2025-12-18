#include "hold_task.h"

using namespace async_flow::frmwork;


Task<Status> HoldTaskHandler::HandleInput(std::shared_ptr<api::HoldTasksReq>& reqBody) {
    if (reqBody->task_type().empty()) {
        
    }
}

Task<Status> HoldTaskHandler::HandleProcess(std::shared_ptr<api::HoldTasksReq>& reqBody, api::HoldTasksRsp& rspBody) {

}
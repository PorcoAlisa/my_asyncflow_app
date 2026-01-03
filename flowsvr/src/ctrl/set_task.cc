#include "set_task.h"
#include "dao.h"
#include "task.h"


namespace async_flow {
namespace flowsvr {

using namespace frmwork;
using namespace db;

using TLarkTask1 = drogon_model::data0::TLarkTask1;

drogon::Task<async_flow::frmwork::Status> SetTaskHandler::HandleInput(std::shared_ptr<api::SetTaskReq>& reqBody) {
    if (reqBody->task_id().empty()) {
        LOG_ERROR << "task id is empty";
        co_return InputInvalid;
    }
    co_return Status::OK;
}

drogon::Task<std::pair<api::SetTaskRsp, Status>> SetTaskHandler::HandleProcess(std::shared_ptr<api::SetTaskReq>& reqBody) {
    if (reqBody->order_time() == 0) reqBody->set_order_time(TimestampNow());

    TLarkTask1 task;
    task.setTaskId(reqBody->task_id());
    task.setUserId(reqBody->user_id());
    task.setTaskType(reqBody->task_type());
    task.setTaskStage(reqBody->task_stage());
    task.setStatus(reqBody->status());
    task.setCrtRetryNum(reqBody->crt_retry_num());
    task.setMaxRetryNum(reqBody->max_retry_num());
    task.setMaxRetryInterval(reqBody->max_retry_interval());
    task.setScheduleLog(reqBody->schedule_log());
    task.setTaskContext(reqBody->task_context());
    task.setOrderTime(reqBody->order_time());

    TaskDao taskDao;
    Status status = co_await taskDao.SaveAsync(task);
    if (!status.ok()) {
        co_return {{}, status};
    }
    co_return {{}, Status::OK};
}

} // flowsvr
} // async_flow
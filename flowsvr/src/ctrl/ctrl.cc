#include "ctrl.h"

using TLarkTask1 = drogon_model::data0::TLarkTask1;

void FillDBTaskModel(const api::TaskData& taskdata, TLarkTask1& ttask) {
    ttask.setUserId(taskdata.user_id());
    ttask.setStatus(taskdata.status());
    ttask.setScheduleLog(taskdata.schedule_log());
    ttask.setTaskStage(taskdata.task_stage());
    ttask.setCrtRetryNum(taskdata.crt_retry_num());
    ttask.setMaxRetryNum(taskdata.max_retry_num());
    ttask.setMaxRetryInterval(taskdata.max_retry_interval());
    ttask.setTaskContext(taskdata.task_context());
    ttask.setOrderTime(taskdata.order_time());
}
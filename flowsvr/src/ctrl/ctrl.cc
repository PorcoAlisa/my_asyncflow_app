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

void FillPBTaskModel(const TLarkTask1& ttask, api::TaskData& taskdata) {
    taskdata.set_user_id(ttask.getValueOfUserId());
    taskdata.set_task_id(ttask.getValueOfTaskId());
    taskdata.set_status(ttask.getValueOfStatus());
    taskdata.set_schedule_log(ttask.getValueOfScheduleLog());
    taskdata.set_task_stage(ttask.getValueOfTaskStage());
    taskdata.set_crt_retry_num(ttask.getValueOfCrtRetryNum());
    taskdata.set_max_retry_num(ttask.getValueOfMaxRetryNum());
    taskdata.set_max_retry_interval(ttask.getValueOfMaxRetryInterval());
    taskdata.set_task_context(ttask.getValueOfTaskContext());
    taskdata.set_order_time(ttask.getValueOfOrderTime());
}

void FillDBScheduleCfgModel(const drogon_model::data0::TScheduleCfg& tcfg, api::TaskScheduleCfg& cfg) {
    cfg.set_task_type(tcfg.getValueOfTaskType());
    cfg.set_schedule_limit(tcfg.getValueOfScheduleLimit());
    cfg.set_schedule_interval(tcfg.getValueOfScheduleInterval());
    cfg.set_max_processing_time(tcfg.getValueOfMaxProcessingTime());
    cfg.set_max_retry_num(tcfg.getValueOfMaxRetryNum());
    cfg.set_max_retry_interval(tcfg.getValueOfRetryInterval());
    cfg.set_create_time(tcfg.getValueOfCreateTime().toFormattedString(false));
    cfg.set_modify_time(tcfg.getValueOfModifyTime().toFormattedString(false));
}
#include "schedule.h"
#include <drogon/drogon.h>
#include <trantor/net/EventLoop.h>

namespace async_flow::worker {

using namespace frmwork;

drogon::Task<> TaskMgr::LoadCfgLoop() {
    auto* taskRpc = drogon::app().getPlugin<TaskRpc>();
    if (!taskRpc) {
        LOG_ERROR << "TaskRpc plugin not found! Did you add it to config.json?";
        co_return;
    }
    while (true) {
        api::GetTaskScheduleCfgListReq req;
        auto [rsp, status] = co_await taskRpc->GetTaskScheduleCfgList(req);
        if (!status.ok()) {
            // 目前错误返回的都是FAIL，error_code都是一样的，这里没啥意义
            LOG_ERROR << "GetTaskScheduleCfgList err: " << status.error_code();
            co_await drogon::sleepCoro(trantor::EventLoop::getEventLoopOfCurrentThread(), 5.0);
            continue;
        }
        std::map<std::string, api::TaskScheduleCfg> newMap;
        LOG_INFO << "schedule cfg list size: " << rsp.task_schedule_cfg_size();
        for (int i = 0; i < rsp.task_schedule_cfg_size(); ++i) {
            newMap[rsp.task_schedule_cfg(i).task_type()] = rsp.task_schedule_cfg(i);
        }
        {
            std::scoped_lock guard(lock_);
            cfgMap_ = newMap;
        }
        LOG_INFO << "update task schedule done";
        co_await drogon::sleepCoro(trantor::EventLoop::getEventLoopOfCurrentThread(), 30 * 60.0);
    }
}

void TaskMgr::Init() {
    LOG_INFO << "TaskMgr Init";

    auto self = shared_from_this();
    drogon::app().registerBeginningAdvice([self] {
        LOG_INFO << "App running, now starting LoadCfgLoop...";
        drogon::async_run([self]() -> drogon::Task<> { return self->LoadCfgLoop(); });
    });
    drogon::app().registerBeginningAdvice([self] {
        LOG_INFO << "begin to Schedule";
        drogon::async_run([self]() -> drogon::Task<> { return self->Schedule(); });
    });
}

drogon::Task<> TaskMgr::Schedule() {
    while (true) {
        api::TaskScheduleCfg scheduleCfg;
        bool cfgReady = false;
        {
            std::scoped_lock guard(lock_);
            if (auto it = cfgMap_.find(taskType_); it != cfgMap_.end()) {
                scheduleCfg = it->second;
                cfgReady = true;
            }
        }

        if (!cfgReady) {
            LOG_INFO << "Task type " << taskType_ << " config not ready, waiting...";
            co_await drogon::sleepCoro(trantor::EventLoop::getEventLoopOfCurrentThread(), 1.0);
            continue;
        }

        std::vector<TaskPtr> tasks = co_await Hold(scheduleCfg);

        size_t numLoops = drogon::app().getThreadNum();
        if (numLoops == 0) numLoops = 1;

        LOG_INFO << "Dispatching " << tasks.size() << " tasks to " << numLoops << " threads.";

        for (size_t i = 0; i < tasks.size(); ++i) {
            trantor::EventLoop* targetLoop = drogon::app().getIOLoop(i % numLoops);
            if (!targetLoop) targetLoop = drogon::app().getLoop();

            const TaskPtr& currentTask = tasks[i];

            targetLoop->queueInLoop([currentTask, scheduleCfg] {
                drogon::async_run([currentTask, scheduleCfg]() -> drogon::Task<> {
                    return RunTask(scheduleCfg, currentTask);
                });
            });
        }
        co_await drogon::sleepCoro(trantor::EventLoop::getEventLoopOfCurrentThread(), scheduleCfg.schedule_interval());
    }
}

drogon::Task<> EndProcess(api::TaskData& taskData, const TaskPtr& taskPtr) {
    if (taskData.status() == TASK_FAILED) {
        Status status = co_await taskPtr->HandleFailedMust();
        if (!status.ok()) {
            taskData.set_status(TASK_PROCESSING);
            LOG_ERROR << "HandleFailedMust error" << status.error_code();
            co_return;
        }
        
        status = co_await taskPtr->HandleFinishError();
        if (!status.ok()) {
            LOG_ERROR << "HandleFinishError error" << status.error_code();
            co_return;
        }
    }
    if (taskData.status() == TASK_FAILED || taskData.status() == TASK_SUCC) {
        co_await taskPtr->HandleFinish();
    }
    if (const Status status = co_await taskPtr->SetTask(); !status.ok()) {
        co_await taskPtr->HandleFinish();
    }
    co_return;
}

drogon::Task<> TaskMgr::RunTask(api::TaskScheduleCfg cfg, TaskPtr taskPtr) {
    api::TaskData& taskData = taskPtr->TaskData();
    Status status = co_await taskPtr->ContextLoad();
    if (!status.ok()) {
        LOG_ERROR << "context load error " << status.error_code();
        co_await EndProcess(taskData, taskPtr);
    }
    status = co_await taskPtr->HandleProcess();

    if (taskData.status() == TASK_PROCESSING) {
        taskData.set_status(TASK_PENDING);
    }
    if (!status.ok()) {
        // 下面这里其实有风险，一个uint32_t + int32_t，然后被转换成int32_t传入set_order_time，有截断风险
        // 我查了一下目前的时间，到2038年的时候，int32就不够用了
        taskData.set_order_time(static_cast<int32_t>(TimestampNow()) + cfg.max_retry_interval());
        if (taskData.max_retry_num() == 0 || taskData.crt_retry_num() >= taskData.max_retry_num()) {
            taskData.set_status(TASK_FAILED);
            co_await EndProcess(taskData, taskPtr);
            co_return;
        }
        if (taskData.status() != TASK_FAILED) {
            taskData.set_crt_retry_num(taskData.crt_retry_num() + 1);
        }
        LOG_ERROR << "HandleProcess error " << status.error_code();
    }
    co_await EndProcess(taskData, taskPtr);
}

drogon::Task<std::vector<TaskPtr>> TaskMgr::Hold(const api::TaskScheduleCfg& cfg) {
    auto* taskRpc = drogon::app().getPlugin<TaskRpc>();
    if (!taskRpc) {
        LOG_ERROR << "TaskRpc plugin not found! Did you add it to config.json?";
        co_return {};
    }

    api::HoldTasksReq req;
    req.set_task_type(taskType_);
    req.set_limit(cfg.schedule_limit());

    auto [rsp, status] = co_await taskRpc->HoldTasks(req);
    if (!status.ok()) {
        LOG_ERROR << "hold task failed.";
        co_return {};
    }
    if (rsp.task_list_size() == 0) {
        LOG_INFO << "hold task is empty.";
        co_return {};
    }
    std::vector<TaskPtr> tasks;
    tasks.reserve(rsp.task_list_size());
    for (int i = 0; i < rsp.task_list_size(); ++i) {
        tasks.emplace_back(factory_(std::move(*rsp.mutable_task_list(i)), taskSvrHost_));
    }
    co_return tasks;
}

}
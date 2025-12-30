#include "schedule.h"
#include <drogon/drogon.h>
#include <trantor/net/EventLoop.h>

namespace async_flow {
namespace worker {

using namespace frmwork;

drogon::Task<void> TaskMgr::LoadCfgLoop() {
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
            std::lock_guard<std::mutex> guard(lock_);
            cfgMap_ = newMap;
        }
        LOG_INFO << "update task schedule done";
        co_await drogon::sleepCoro(trantor::EventLoop::getEventLoopOfCurrentThread(), 30 * 60.0);
    }
}

void TaskMgr::Init() {
    LOG_INFO << "TaskMgr Init";

    auto self = shared_from_this();
    drogon::app().registerBeginningAdvice([self]() {
        LOG_INFO << "App running, now starting LoadCfgLoop...";
        drogon::async_run([self]() -> drogon::Task<void> { return self->LoadCfgLoop(); });
    });
}

void TaskMgr::Schedule() {
    LOG_INFO << "begin to Schedule";
    auto self = shared_from_this();
    drogon::app().registerBeginningAdvice([self]() {
        drogon::async_run([self]() -> drogon::Task<void> {
            api::TaskScheduleCfg scheduleCfg;
            {
                std::lock_guard<std::mutex> guard(self->lock_);
                auto it = self->cfgMap_.find(self->taskType_);
                if (it != self->cfgMap_.end()) {
                    scheduleCfg = it->second;
                } else {
                    LOG_ERROR << "Task type " << self->taskType_ << " not found in cfgMap_";
                    co_return;
                }
            }
            std::vector<TaskPtr> tasks = co_await self->Hold(scheduleCfg);

            size_t numLoops = drogon::app().getThreadNum();
            if (numLoops == 0) numLoops = 1;

            LOG_INFO << "Dispatching " << tasks.size() << " tasks to " << numLoops << " threads.";

            for (size_t i = 0; i < tasks.size(); ++i) {
                trantor::EventLoop* targetLoop = drogon::app().getIOLoop(i % numLoops);
                if (!targetLoop) targetLoop = drogon::app().getLoop();

                TaskPtr currentTask = tasks[i];

                targetLoop->queueInLoop([self, currentTask, targetLoop, scheduleCfg]() -> drogon::Task<void> {
                    return self->RunTask(scheduleCfg, currentTask);
                });
            }
        });
    });
}

drogon::Task<void> TaskMgr::RunTask(const api::TaskScheduleCfg& cfg, TaskPtr taskPtr) {
    co_return;
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
    for (size_t i = 0; i < rsp.task_list_size(); ++i) {
        tasks.emplace_back(factory_(std::move(rsp.task_list(i)), taskSvrHost_));
    }
    co_return tasks;
}

}
}
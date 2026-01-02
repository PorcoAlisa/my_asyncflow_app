#pragma once

#include "api.pb.h"
#include "status.h"
#include "comm.h"
#include <functional>
#include <drogon/drogon.h>
#include <ctime>

namespace async_flow::worker {

class TaskRpc : public drogon::Plugin<TaskRpc> {
public:
    drogon::Task<std::pair<api::GetTaskScheduleCfgListRsp, frmwork::Status>> GetTaskScheduleCfgList(const api::GetTaskScheduleCfgListReq& req);
    drogon::Task<std::pair<api::HoldTasksRsp, frmwork::Status>> HoldTasks(const api::HoldTasksReq& req);
    drogon::Task<std::pair<api::SetTaskRsp, frmwork::Status>> SetTask(const api::SetTaskReq& req);
    drogon::Task<std::pair<api::CreateTaskRsp, frmwork::Status>> CreateTask(const api::CreateTaskReq& req);
private:
    void initAndStart(const Json::Value &config) override {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        host_ = config.get("host", "http://127.0.0.1:39002").asString();
        size_t threadNum = drogon::app().getThreadNum();
        LOG_INFO << "Init and Start: threadNum: " << threadNum;
        if (threadNum == 0) threadNum = 1;
        clients_.reserve(threadNum);

        for (size_t i = 0; i < threadNum; ++i) {
            trantor::EventLoop* loop = drogon::app().getIOLoop(i);
            drogon::HttpClientPtr client = drogon::HttpClient::newHttpClient(host_, loop);
            client->setPipeliningDepth(10);
            clients_.push_back(client);

            LOG_INFO << "TaskRpc initialized with " << clients_.size() << " clients.";
        }
    }

    void shutdown() override {
        LOG_INFO << "TaskRpc Shutting down";
    }

    template<class ReqBody, class RspBody>
    drogon::Task<std::pair<RspBody, async_flow::frmwork::Status>> SendRequest(const std::string& url, const ReqBody& reqBody) {
        RspBody rspBody; // 默认构造
        std::string stReqBody;
        if (!ProtobufUtil::MessageToJsonString(reqBody, &stReqBody).ok()) {
            LOG_ERROR << "Protobuf serialization failed";
            co_return {rspBody, frmwork::Status::FAIL};
        }

        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(drogon::Post);
        req->setPath(url);
        req->setBody(std::move(stReqBody));

        try {
            auto client = getRandomClient();
            auto response = co_await client->sendRequestCoro(req);
            if (response->getStatusCode() != drogon::k200OK) {
                LOG_ERROR << "HTTP error: " << response->getStatusCode();
                co_return {rspBody, frmwork::Status::FAIL};
            }

            auto bodyView = response->getBody();
            std::string bodyStr(bodyView.data(), bodyView.length());
            if (!ProtobufUtil::JsonStringToMessage(bodyStr, &rspBody).ok()) {
                LOG_ERROR << "JSON parsing failed. Body: " << bodyStr;
                co_return {rspBody, frmwork::Status::FAIL};
            }
            co_return {rspBody, frmwork::Status::OK};
        } catch (const std::exception& e) {
            LOG_ERROR << "Network or General error: " << e.what();
            co_return {rspBody, frmwork::Status::FAIL};
        }
    }

private:
    drogon::HttpClientPtr getRandomClient() {
        if (clients_.empty()) return nullptr;
        size_t idx = std::rand() % clients_.size();
        return clients_[idx];
    }

private:
    std::vector<drogon::HttpClientPtr> clients_;
    std::string host_;
};

class TaskBase {
public:
    TaskBase() = default;
    TaskBase(api::TaskData taskData, const std::string_view host) : taskData_(std::move(taskData)), svr_host_(host) {}
    virtual ~TaskBase() = default;
    virtual drogon::Task<frmwork::Status> ContextLoad() { co_return frmwork::Status::OK; }
    virtual drogon::Task<frmwork::Status> HandleProcess() { co_return frmwork::Status::OK; }
    virtual drogon::Task<frmwork::Status> HandleFailedMust() { co_return frmwork::Status::OK; }
    virtual drogon::Task<frmwork::Status> HandleFinishError() { co_return frmwork::Status::OK; }
    virtual drogon::Task<void> HandleFinish() { co_return; }
    virtual drogon::Task<frmwork::Status> SetTask();
    api::TaskData& TaskData() { return taskData_; }
protected:
    api::TaskData taskData_;
    std::string svr_host_;
};

using TaskPtr = std::shared_ptr<TaskBase>;
using TaskBaseFactory = std::function<TaskPtr(const api::TaskData&, std::string_view)>;

}
#pragma once

#include "api.pb.h"
#include "status.h"
#include "comm.h"
#include <functional>
#include <drogon/drogon.h>

namespace async_flow {
namespace worker {

class TaskRpc : public drogon::Plugin<TaskRpc> {
public:
    drogon::Task<std::pair<api::GetTaskScheduleCfgListRsp, frmwork::Status>> GetTaskScheduleCfgList(const api::GetTaskScheduleCfgListReq& req);
    drogon::Task<std::pair<api::HoldTasksRsp, frmwork::Status>> HoldTasks(const api::HoldTasksReq& req);
    drogon::Task<std::pair<api::SetTaskRsp, frmwork::Status>> SetTask(const api::SetTaskReq& req);
    drogon::Task<std::pair<api::CreateTaskRsp, frmwork::Status>> CreateTask(const api::CreateTaskReq& req);
private:
    void initAndStart(const Json::Value &config) override {
        host_ = config.get("host", "http://127.0.0.1:39002").asString();
        client_ = drogon::HttpClient::newHttpClient(host_);
        client_->setPipeliningDepth(10);
        LOG_INFO << "TaskRpc initialized with host: " << host_;
    }
    void shutdown() override {
        LOG_INFO << "TaskRpc Shutting down";
    }
    template<class ReqBody, class RspBody>
    drogon::Task<std::pair<RspBody, async_flow::frmwork::Status>> SendRequest(const std::string& url, const ReqBody& reqBody) {
        std::string stReqBody;
        ProtobufUtil::MessageToJsonString(reqBody, &stReqBody);
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(drogon::Post);
        req->setPath(url);
        req->setBody(stReqBody);
        RspBody rspBody;
        try {
            auto response = co_await client_->sendRequestCoro(req);
            if (!ProtobufUtil::JsonStringToMessage(std::string(response->getBody().data()), &rspBody).ok()) {
                co_return {rspBody, frmwork::Status(rspBody.code(), rspBody.msg())};
            }
        } catch (const std::exception& e) {
            LOG_ERROR << "Network or General error: " << e.what();
            co_return {rspBody, frmwork::Status::FAIL};
        }
        co_return {rspBody, frmwork::Status::OK};
    }

private:
    drogon::HttpClientPtr client_;
    std::string host_;
};

class TaskBase {
public:
    TaskBase() {}
    TaskBase(const api::TaskData& taskData, std::string_view host) : taskData_(taskData), svr_host_(host) {}
    virtual ~TaskBase() = default;
protected:
    api::TaskData taskData_;
    std::string svr_host_;
};

using TaskPtr = std::shared_ptr<TaskBase>;
using TaskBaseFactory = std::function<TaskPtr(const api::TaskData&, std::string_view)>;

}
}
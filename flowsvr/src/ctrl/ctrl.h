#pragma once
#include <string>
#include <drogon/drogon.h>
#include "status.h"
#include "api.pb.h"
#include "TLarkTask1.h"
#include "const.h"
#include "TScheduleCfg.h"
#include "comm.h"
#include <google/protobuf/util/json_util.h>
#include <absl/status/status.h>


namespace async_flow {
namespace flowsvr {

template <class handler_t>
void RegisterHandler(drogon::HttpAppFramework& app, const std::string& path) {
    app.registerHandler(path, [](drogon::HttpRequestPtr req) -> drogon::Task<drogon::HttpResponsePtr> {
        auto h = std::make_shared<handler_t>();
        co_return co_await h->ProcessCoro(req);
    });
}


template <class ReqProto, class RspProto>
class Handler : public std::enable_shared_from_this<Handler<ReqProto, RspProto>> {
public:
    virtual ~Handler() = default;

    virtual drogon::Task<async_flow::frmwork::Status> HandleInput(std::shared_ptr<ReqProto>&) = 0;
    virtual drogon::Task<std::pair<RspProto, async_flow::frmwork::Status>> HandleProcess(std::shared_ptr<ReqProto>&) = 0;

    drogon::HttpResponsePtr Reply(int code, const std::string& msg, RspProto& rsp);
    drogon::HttpResponsePtr ReplySucc(RspProto& rsp);
    drogon::HttpResponsePtr ReplyFail(async_flow::frmwork::Status status, RspProto& rsp);

    drogon::Task<drogon::HttpResponsePtr> ProcessCoro(drogon::HttpRequestPtr reqHTTP);
};

template<class ReqProto, class RspProto>
drogon::HttpResponsePtr Handler<ReqProto, RspProto>::Reply(int code, const std::string& msg, RspProto& rsp) {
    rsp.set_code(code);
    rsp.set_msg(msg);
    std::string strReply;
    google::protobuf::util::JsonPrintOptions opt;
    opt.always_print_fields_with_no_presence = true;
    absl::Status status = google::protobuf::util::MessageToJsonString(rsp, &strReply, opt);
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(strReply);
    resp->setStatusCode(drogon::k200OK);
    return resp;
}

template<class ReqProto, class RspProto>
drogon::HttpResponsePtr Handler<ReqProto, RspProto>::ReplySucc(RspProto& rsp) {
    return Reply(async_flow::frmwork::Status::OK.error_code(), async_flow::frmwork::Status::OK.error_message(), rsp);
}

template<class ReqProto, class RspProto>
drogon::HttpResponsePtr Handler<ReqProto, RspProto>::ReplyFail(async_flow::frmwork::Status status, RspProto& rsp) {
    return Reply(status.error_code(), status.error_message(), rsp);
}

template<class ReqProto, class RspProto>
drogon::Task<drogon::HttpResponsePtr> Handler<ReqProto, RspProto>::ProcessCoro(drogon::HttpRequestPtr reqHTTP) {
    auto self = this->shared_from_this();
    std::shared_ptr<ReqProto> reqPtr = std::make_shared<ReqProto>();
    
    // JSON 解析
    absl::Status status1 = google::protobuf::util::JsonStringToMessage(reqHTTP->bodyData(), reqPtr.get());
    if (!status1.ok()) {
        RspProto rsp;
        // 【修改点】：以前是 callback(...)，现在直接 co_return
        co_return self->ReplyFail(async_flow::frmwork::InputInvalid, rsp);
    }

    // 1. HandleInput
    auto status2 = co_await self->HandleInput(reqPtr);
    if (!status2.ok()) {
        RspProto failedRspBody;
        // 【修改点】：直接 co_return
        co_return self->ReplyFail(status2, failedRspBody);
    }
    
    // 2. HandleProcess
    auto [rspBody, status3] = co_await self->HandleProcess(reqPtr);
    if (!status3.ok()) {
        // 【修改点】：直接 co_return
        co_return self->ReplyFail(status3, rspBody);
    }

    // 3. 成功
    // 【修改点】：直接 co_return
    co_return self->ReplySucc(rspBody);
}

} // namespace flowsvr
} // namespace async_flow

void FillDBTaskModel(const api::TaskData& taskdata, drogon_model::data0::TLarkTask1& ttask);
void FillPBTaskModel(const drogon_model::data0::TLarkTask1& ttask, api::TaskData& taskdata);
void FillDBScheduleCfgModel(const drogon_model::data0::TScheduleCfg& tcfg, api::TaskScheduleCfg& cfg);
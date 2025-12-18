#pragma once
#include <string>
#include <drogon/drogon.h>
#include "handler_base.h"
#include "status.h"
#include "api.pb.h"
#include "TLarkTask1.h"
#include "const.h"


namespace async_flow {
namespace flowsvr {

template <class handler_t>
void RegisterHandler(HttpAppFramework& app, const std::string& path) {
    app.registerHandler(path, [](HttpRequestPtr req) -> Task<HttpResponsePtr> {
        auto h = std::make_shared<handler_t>();
        co_return co_await h->ProcessCoro(req);
    });
}


template <class ReqProto, class RspProto>
class Handler : public std::enable_shared_from_this<Handler<ReqProto, RspProto>> {
public:
    virtual ~Handler() = default;

    virtual Task<async_flow::frmwork::Status> HandleInput(std::shared_ptr<ReqProto>&) = 0;
    virtual Task<async_flow::frmwork::Status> HandleProcess(std::shared_ptr<ReqProto>&, RspProto&) = 0;

    HttpResponsePtr Reply(int code, const std::string& msg, RspProto& rsp);
    HttpResponsePtr ReplySucc(RspProto& rsp);
    HttpResponsePtr ReplyFail(async_flow::frmwork::Status status, RspProto& rsp);

    Task<HttpResponsePtr> ProcessCoro(HttpRequestPtr reqHTTP);
};

template<class ReqProto, class RspProto>
HttpResponsePtr Handler<ReqProto, RspProto>::Reply(int code, const std::string& msg, RspProto& rsp) {
    rsp.set_code(code);
    rsp.set_msg(msg);
    std::string strReply;
    ProtobufUtil::JsonPrintOptions opt;
    opt.always_print_primitive_fields = true;
    ProtobufUtil::Status status = ProtobufUtil::MessageToJsonString(rsp, &strReply, opt);
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody(strReply);
    resp->setStatusCode(k200OK);
    return resp;
}

template<class ReqProto, class RspProto>
HttpResponsePtr Handler<ReqProto, RspProto>::ReplySucc(RspProto& rsp) {
    return Reply(async_flow::frmwork::Status::OK.error_code(), async_flow::frmwork::Status::OK.error_message(), rsp);
}

template<class ReqProto, class RspProto>
HttpResponsePtr Handler<ReqProto, RspProto>::ReplyFail(async_flow::frmwork::Status status, RspProto& rsp) {
    return Reply(status.error_code(), status.error_message(), rsp);
}

template<class ReqProto, class RspProto>
Task<HttpResponsePtr> Handler<ReqProto, RspProto>::ProcessCoro(HttpRequestPtr reqHTTP) {
    auto self = this->shared_from_this();
    std::shared_ptr<ReqProto> reqPtr = std::make_shared<ReqProto>();
    
    // JSON 解析
    ProtobufUtil::Status status = ProtobufUtil::JsonStringToMessage(reqHTTP->bodyData(), reqPtr.get());
    if (!status.ok()) {
        RspProto rsp;
        // 【修改点】：以前是 callback(...)，现在直接 co_return
        co_return self->ReplyFail(async_flow::frmwork::InputInvalid, rsp);
    }

    RspProto rspBody;
    async_flow::frmwork::Status flowStatus;

    // 1. HandleInput
    flowStatus = co_await self->HandleInput(reqPtr);
    if (!flowStatus.ok()) {
        // 【修改点】：直接 co_return
        co_return self->ReplyFail(flowStatus, rspBody);
    }
    
    // 2. HandleProcess
    flowStatus = co_await self->HandleProcess(reqPtr, rspBody);
    if (!flowStatus.ok()) {
        // 【修改点】：直接 co_return
        co_return self->ReplyFail(flowStatus, rspBody);
    }

    // 3. 成功
    // 【修改点】：直接 co_return
    co_return self->ReplySucc(rspBody);
}

} // namespace flowsvr
} // namespace async_flow

void FillDBTaskModel(const api::TaskData& taskdata, drogon_model::data0::TLarkTask1& ttask);
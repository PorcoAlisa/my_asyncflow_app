#include "status.h"

using namespace async_flow::frmwork;

const Status Status::OK = Status(error::OK, "success");
const Status Status::FAIL = Status(error::FAILED, "failed");

Status::Status() {
  error_code_ = error::OK;
  error_message_ = "success";
}

Status::Status(const Status& s) {
  error_code_ = s.error_code();
  error_message_ = s.error_message();
}
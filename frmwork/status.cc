#include "status.h"

using namespace async_flow::frmwork;

const Status Status::OK = Status(Error::OK, "success");
const Status Status::FAIL = Status(Error::FAILED, "failed");

Status::Status() {
  error_code_ = Error::OK;
  error_message_ = "success";
}

Status::Status(const Status& s) {
  error_code_ = s.error_code();
  error_message_ = s.error_message();
}
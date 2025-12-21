#include "const.h"

namespace async_flow {
namespace frmwork {

const Status InputInvalid = Status(ERR_INPUT_INVALID, "input invalid");
const Status JsonMarshalErr = Status(ERR_JSON_MARSHAL, "should bind failed");
const Status DBExecErr = Status(ERR_DB_EXEC_ERR, "db exec err");
const Status ResourceNotFound = Status(ERR_RESOURCE_NOT_FOUND, "resource not found");

} // namespace frmwork
} // namespace async_flow
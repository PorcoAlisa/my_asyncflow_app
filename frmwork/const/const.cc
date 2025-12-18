#include "const.h"

using namespace async_flow::frmwork;

Status InputInvalid = Status(ERR_INPUT_INVALID, "input invalid");
Status JsonMarshalErr = Status(ERR_JSON_MARSHAL, "should bind failed");
Status DBExecErr = Status(ERR_DB_EXEC_ERR, "db exec err");
Status ResourceNotFound = Status(ERR_RESOURCE_NOT_FOUND, "resource not found");
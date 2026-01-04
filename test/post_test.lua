-- post_test.lua

wrk.method = "POST"

wrk.body = 
[[{
  "taskData": {
    "userId": "user_12345",
    "taskType": "lark",
    "taskStage": "processing",
    "status": 0,
    "crtRetryNum": 0,
    "scheduleLog": "初始化任务...",
    "context": "{\"meta\": \"data\"}"
  }
}]]
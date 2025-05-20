# llm-vad

语音活动检测单元，用于提供识别音频信号中是否存在语音成分服务。

## setup

配置单元工作。

发送 json：

```json
{
  "request_id": "2",
  "work_id": "vad",
  "action": "setup",
  "object": "vad.setup",
  "data": {
    "model": "silero-vad",
    "response_format": "vad.bool",
    "input": "sys.pcm",
    "enoutput": true
  }
}
```

- request_id：参考基本数据解释。
- work_id：配置单元时，为 `vad`。
- action：调用的方法为 `setup`。
- object：传输的数据类型为 `vad.setup`。
- model：使用的模型为 `silero-vad` 中文模型。
- response_format：返回结果为 `vad.bool` 型。
- input：输入的为 `sys.pcm`,代表的是系统音频。
- enoutput：是否起用用户结果输出。

响应 json：

```json
{
  "created": 1737595236,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "2",
  "work_id": "vad.1000"
}
```

- created：消息创建时间，unix 时间。
- work_id：返回成功创建的 work_id 单元。

## pause

暂停单元工作。

发送 json：

```json
{
  "request_id": "3",
  "work_id": "vad.1000",
  "action": "pause"
}
```

响应 json：

```json
{
  "created": 1737595314,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "3",
  "work_id": "vad.1000"
}
```

error::code 为 0 表示执行成功。

## work

恢复单元工作。

发送 json：

```json
{
  "request_id": "4",
  "work_id": "vad.1000",
  "action": "work"
}
```

响应 json：

```json
{
  "created": 1737595419,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "4",
  "work_id": "vad.1000"
}
```

error::code 为 0 表示执行成功。

## exit

单元退出。

发送 json：

```json
{
  "request_id": "5",
  "work_id": "vad.1000",
  "action": "exit"
}
```

响应 json：

```json
{
  "created": 1737595478,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "5",
  "work_id": "vad.1000"
}
```

error::code 为 0 表示执行成功。

## taskinfo

获取任务列表。

发送 json：

```json
{
  "request_id": "2",
  "work_id": "vad",
  "action": "taskinfo"
}
```

响应 json：

```json
{
  "created": 1737595606,
  "data": [
    "vad.1000"
  ],
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "vad.tasklist",
  "request_id": "2",
  "work_id": "vad"
}
```

获取任务运行参数。

```json
{
  "request_id": "2",
  "work_id": "vad.1000",
  "action": "taskinfo"
}
```

响应 json：

```json
{
  "created": 1737596100,
  "data": {
    "enoutput": true,
    "inputs": [
      "sys.pcm"
    ],
    "model": "silero-vad",
    "response_format": "vad.bool"
  },
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "vad.taskinfo",
  "request_id": "2",
  "work_id": "vad.1000"
}
```

> **注意：work_id 是按照单元的初始化注册顺序增加的，并不是固定的索引值。**

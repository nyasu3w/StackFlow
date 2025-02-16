# llm-asr

语音转文字单元，可选择模型识别语言，用于提供多种语言的语音转文字服务。

## setup

配置单元工作。

发送 json：

```json
{
  "request_id": "2",
  "work_id": "whisper",
  "action": "setup",
  "object": "whisper.setup",
  "data": {
    "model": "whisper-tiny",
    "response_format": "asr.utf-8",
    "input": "sys.pcm",
    "language": "zh",
    "enoutput": true
  }
}
```

- request_id：参考基本数据解释。
- work_id：配置单元时，为 `whisper`。
- action：调用的方法为 `setup`。
- object：传输的数据类型为 `whisper.setup`。
- model：使用的模型为 `whisper-tiny` 模型。
- response_format：返回结果为 `asr.utf-8`, utf-8 的非流式输出。
- input：输入的为 `sys.pcm`,代表的是系统音频。
- language:选择模型识别的语言。
- enoutput：是否起用用户结果输出。

响应 json：

```json
{
  "created": 1737597583,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "2",
  "work_id": "whisper.1002"
}
```

- created：消息创建时间，unix 时间。
- work_id：返回成功创建的 work_id 单元。

## link

链接上级单元的输出。

发送 json：

```json
{
  "request_id": "3",
  "work_id": "whisper.1002",
  "action": "link",
  "object": "work_id",
  "data": "vad.1000"
}
```

响应 json：

```json
{
  "created": 1737597688,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "3",
  "work_id": "whisper.1002"
}
```

error::code 为 0 表示执行成功。

将 vad 和 whisper 单元链接起来，当 vad 发出检测到语言数据开始时，whisper 单元开始识别用户的语音，当 vad 发出检测到语言结束时，whisper
单元输出识别结果，直到下一次 vad 检测到语音。

将 kws 和 whisper 单元链接起来，当 kws 发出唤醒数据时，whisper 单元开始识别用户的语音，当 vad 发出检测到语言结束时，whisper
单元输出识别结果，直到下一次的唤醒。

> **link 时必须保证 kws 和 vad 此时已经配置好进入工作状态，且 kws 和 vad 已经链接，详见 vad 文档。link 也可以在 setup 阶段进行。
**

示例:

```json
{
  "request_id": "2",
  "work_id": "whisper",
  "action": "setup",
  "object": "whisper.setup",
  "data": {
    "model": "whisper-tiny",
    "response_format": "asr.utf-8",
    "input": [
      "sys.pcm",
      "kws.1000",
      "vad.1001"
    ],
    "language": "zh",
    "enoutput": true
  }
}
```

## unlink

取消链接。

发送 json：

```json
{
  "request_id": "4",
  "work_id": "whisper.1002",
  "action": "unlink",
  "object": "work_id",
  "data": "vad.1000"
}
```

响应 json：

```json
{
  "created": 1737598243,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "4",
  "work_id": "whisper.1002"
}
```

error::code 为 0 表示执行成功。

## pause

暂停单元工作。

发送 json：

```json
{
  "request_id": "5",
  "work_id": "whisper.1002",
  "action": "pause"
}
```

响应 json：

```json
{
  "created": 1737598297,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "5",
  "work_id": "whisper.1002"
}
```

error::code 为 0 表示执行成功。

## work

恢复单元工作。

发送 json：

```json
{
  "request_id": "6",
  "work_id": "whisper.1002",
  "action": "work"
}
```

响应 json：

```json
{
  "created": 1737598333,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "6",
  "work_id": "whisper.1002"
}
```

error::code 为 0 表示执行成功。

## exit

单元退出。

发送 json：

```json
{
  "request_id": "7",
  "work_id": "whisper.1002",
  "action": "exit"
}
```

响应 json：

```json
{
  "created": 1737598447,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "7",
  "work_id": "whisper.1002"
}
```

error::code 为 0 表示执行成功。

## taskinfo

获取任务列表。

发送 json：

```json
{
  "request_id": "2",
  "work_id": "whisper",
  "action": "taskinfo"
}
```

响应 json：

```json
{
  "created": 1737598371,
  "data": [
    "whisper.1002"
  ],
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "whisper.tasklist",
  "request_id": "2",
  "work_id": "whisper"
}
```

获取任务运行参数。

```json
{
  "request_id": "2",
  "work_id": "whisper.1002",
  "action": "taskinfo"
}
```

响应 json：

```json
{
  "created": 1737598415,
  "data": {
    "enoutput": true,
    "inputs": [
      "sys.pcm"
    ],
    "model": "whisper-tiny",
    "response_format": "asr.utf-8"
  },
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "whisper.taskinfo",
  "request_id": "2",
  "work_id": "whisper.1002"
}
```

> **注意：work_id 是按照单元的初始化注册顺序增加的，并不是固定的索引值。**
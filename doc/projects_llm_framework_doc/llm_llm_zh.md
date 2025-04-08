# llm-llm

大模型单元，用于提供大模型推理服务。

## setup

配置单元工作。

发送 json：

```json
{
  "request_id": "2",
  "work_id": "llm",
  "action": "setup",
  "object": "llm.setup",
  "data": {
    "model": "qwen2.5-0.5B-prefill-20e",
    "response_format": "llm.utf-8.stream",
    "input": "llm.utf-8",
    "enoutput": true,
    "max_token_len": 256,
    "prompt": "You are a knowledgeable assistant capable of answering various questions and providing information."
  }
}
```

- request_id：参考基本数据解释。
- work_id：配置单元时，为 `llm`。
- action：调用的方法为 `setup`。
- object：传输的数据类型为 `llm.setup`。
- model：使用的模型为 `qwen2.5-0.5B-prefill-20e` 中文模型。
- response_format：返回结果为 `llm.utf-8.stream`, utf-8 的流式输出。
- input：输入的为 `llm.utf-8`,代表的是从用户输入。
- enoutput：是否起用用户结果输出。
- max_token_len：最大输出 token,该值的最大值受到模型的最大限制。
- prompt：模型的系统提示词。

响应 json：

```json
{
  "created": 1731488402,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "2",
  "work_id": "llm.1002"
}
```

- created：消息创建时间，unix 时间。
- work_id：返回成功创建的 work_id 单元。

## inference

### 流式输入

```json
{
    "request_id": "2",
    "work_id": "llm.1003",
    "action": "inference",
    "object": "llm.utf-8.stream",
    "data": {
        "delta": "What's ur name?",
        "index": 0,
        "finish": true
    }
}
```
- object：传输的数据类型为 `llm.utf-8.stream` 代表的是从用户 utf-8 的流式输入
- delta：流式输入的分段数据
- index：流式输入的分段索引
- finish:流式输入是否完成的标志位

### 非流式输入

```json
{
    "request_id": "2",
    "work_id": "llm.1003",
    "action": "inference",
    "object": "llm.utf-8",
    "data": "What's ur name?"
}
```
- object：传输的数据类型为 `llm.utf-8` 代表的是从用户 utf-8 的非流式输入
- data：非流式输入的数据

流式响应 json：

```json
{"created":1742779468,"data":{"delta":"I am not","finish":false,"index":0},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
{"created":1742779469,"data":{"delta":" a person,","finish":false,"index":1},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
{"created":1742779469,"data":{"delta":" but I'm","finish":false,"index":2},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
{"created":1742779470,"data":{"delta":" here to assist","finish":false,"index":3},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
{"created":1742779471,"data":{"delta":" you with any","finish":false,"index":4},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
{"created":1742779472,"data":{"delta":" questions or tasks","finish":false,"index":5},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
{"created":1742779473,"data":{"delta":" you may have","finish":false,"index":6},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
{"created":1742779474,"data":{"delta":". How can","finish":false,"index":7},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
{"created":1742779474,"data":{"delta":" I help you","finish":false,"index":8},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
{"created":1742779475,"data":{"delta":" today?","finish":false,"index":9},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
{"created":1742779475,"data":{"delta":"","finish":true,"index":10},"error":{"code":0,"message":""},"object":"llm.utf-8.stream","request_id":"2","work_id":"llm.1003"}
```

非流式响应 json：

```json
{
    "created": 1742780120,
    "data": "As an artificial intelligence, I don't have a name in the traditional sense. However, I am here to assist you with any questions or information you may need. How can I help you today?",
    "error": {
        "code": 0,
        "message": ""
    },
    "object": "llm.utf-8",
    "request_id": "2",
    "work_id": "llm.1003"
}
```

## link

链接上级单元的输出。

发送 json：

```json
{
  "request_id": "3",
  "work_id": "llm.1002",
  "action": "link",
  "object": "work_id",
  "data": "kws.1000"
}
```

响应 json：

```json
{
  "created": 1731488402,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "3",
  "work_id": "llm.1002"
}
```

error::code 为 0 表示执行成功。

将 llm 和 kws 单元链接起来，当 kws 发出唤醒数据时，llm 单元停止上次未完成的推理，用于重复唤醒功能。

> **link 时必须保证 kws 此时已经配置好进入工作状态。link 也可以在 setup 阶段进行。**

示例:

```json
{
  "request_id": "2",
  "work_id": "llm",
  "action": "setup",
  "object": "llm.setup",
  "data": {
    "model": "qwen2.5-0.5B-prefill-20e",
    "response_format": "llm.utf-8.stream",
    "input": [
      "llm.utf-8",
      "asr.1001",
      "kws.1000"
    ],
    "enoutput": true,
    "max_token_len": 256,
    "prompt": "You are a knowledgeable assistant capable of answering various questions and providing information."
  }
}
```

## unlink

取消链接。

发送 json：

```json
{
  "request_id": "4",
  "work_id": "llm.1002",
  "action": "unlink",
  "object": "work_id",
  "data": "kws.1000"
}
```

响应 json：

```json
{
  "created": 1731488402,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "4",
  "work_id": "llm.1002"
}
```

error::code 为 0 表示执行成功。

## pause

暂停单元工作。

发送 json：

```json
{
  "request_id": "5",
  "work_id": "llm.1002",
  "action": "pause"
}
```

响应 json：

```json
{
  "created": 1731488402,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "5",
  "work_id": "llm.1002"
}
```

error::code 为 0 表示执行成功。

## work

恢复单元工作。

发送 json：

```json
{
  "request_id": "6",
  "work_id": "llm.1002",
  "action": "work"
}
```

响应 json：

```json
{
  "created": 1731488402,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "6",
  "work_id": "llm.1002"
}
```

error::code 为 0 表示执行成功。

## exit

退出单元工作。

发送 json：

```json
{
  "request_id": "7",
  "work_id": "llm.1002",
  "action": "exit"
}
```

响应 json：

```json
{
  "created": 1731488402,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "7",
  "work_id": "llm.1002"
}
```

error::code 为 0 表示执行成功。

## taskinfo

获取任务列表。

发送 json：

```json
{
  "request_id": "2",
  "work_id": "llm",
  "action": "taskinfo"
}
```

响应 json：

```json
{
  "created": 1731652149,
  "data": [
    "llm.1002"
  ],
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "llm.tasklist",
  "request_id": "2",
  "work_id": "llm"
}
```

获取任务运行参数。

```json
{
  "request_id": "2",
  "work_id": "llm.1002",
  "action": "taskinfo"
}
```

响应 json：

```json
{
  "created": 1731652187,
  "data": {
    "enoutput": true,
    "inputs_": [
      "llm.utf-8"
    ],
    "model": "qwen2.5-0.5B-prefill-20e",
    "response_format": "llm.utf-8.stream"
  },
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "llm.taskinfo",
  "request_id": "2",
  "work_id": "llm.1002"
}
```

> **注意：work_id 是按照单元的初始化注册顺序增加的，并不是固定的索引值。**
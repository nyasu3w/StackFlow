# llm-asr

The speech-to-text module, which allows you to choose the language model for recognizing speech, provides speech-to-text
services in multiple languages.

## setup

Configure the module to work.

Send JSON:

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

- `request_id`: Reference basic data explanation.
- `work_id`: For configuration, use `whisper`.
- `action`: The method called is `setup`.
- `object`: The data type being transferred is `whisper.setup`.
- `model`: The model being used is `whisper-tiny`.
- `response_format`: The returned result is `asr.utf-8`, which is a non-streaming UTF-8 output.
- `input`: The input is `sys.pcm`, representing system audio.
- `language`: The language for the model to recognize.
- `enoutput`: Whether to enable user result output.

Response JSON:

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

- `created`: The message creation time, in Unix time.
- `work_id`: The work ID of the successfully created unit.

## link

Link the output of the upper-level unit.

Send JSON:

```json
{
  "request_id": "3",
  "work_id": "whisper.1002",
  "action": "link",
  "object": "work_id",
  "data": "vad.1000"
}
```

Response JSON:

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

Error `code: 0` indicates successful execution.

Link the `vad` and `whisper` units, so when `vad` detects the start of speech data, the `whisper` unit will begin
recognizing the user's speech. When `vad` detects the end of speech, the `whisper` unit will output the recognition
result, continuing until the next speech is detected by `vad`.

Link `kws` and `whisper` units. When `kws` detects a wake-up signal, the `whisper` unit will start recognizing the
user's speech, and when `vad` detects the end of speech, `whisper` will output the result until the next wake-up event.

> **When linking, make sure that `kws` and `vad` are properly configured and linked together. See the `vad`
documentation for more details. Linking can also be done during the setup phase.**

Example:

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

Unlink the units.

Send JSON:

```json
{
  "request_id": "4",
  "work_id": "whisper.1002",
  "action": "unlink",
  "object": "work_id",
  "data": "vad.1000"
}
```

Response JSON:

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

Error `code: 0` indicates successful execution.

## pause

Pause the module.

Send JSON:

```json
{
  "request_id": "5",
  "work_id": "whisper.1002",
  "action": "pause"
}
```

Response JSON:

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

Error `code: 0` indicates successful execution.

## work

Resume the module.

Send JSON:

```json
{
  "request_id": "6",
  "work_id": "whisper.1002",
  "action": "work"
}
```

Response JSON:

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

Error `code: 0` indicates successful execution.

## exit

Exit the module.

Send JSON:

```json
{
  "request_id": "7",
  "work_id": "whisper.1002",
  "action": "exit"
}
```

Response JSON:

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

Error `code: 0` indicates successful execution.

## taskinfo

Get the task list.

Send JSON:

```json
{
  "request_id": "2",
  "work_id": "whisper",
  "action": "taskinfo"
}
```

Response JSON:

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

Get task parameters:

```json
{
  "request_id": "2",
  "work_id": "whisper.1002",
  "action": "taskinfo"
}
```

Response JSON:

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

> **Note: `work_id` increases according to the initialization order of units and is not a fixed index value.**
# llm-vad

The voice activity detection unit is used to provide the service of identifying whether there is speech content in an audio signal.

## setup

Configure the unit for operation.

Send JSON:

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

- request_id: Refers to the basic data explanation.
- work_id: For configuring the unit, use `vad`.
- action: The method called is `setup`.
- object: The data type being transferred is `vad.setup`.
- model: The model used is the Chinese version of `silero-vad`.
- response_format: The returned result is of type `vad.bool`.
- input: The input is `sys.pcm`, which represents system audio.
- enoutput: Whether to enable the output for user results.

Response JSON:

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

- created: Message creation time, in Unix time.
- work_id: The work_id of the successfully created unit.

## pause

Pause the unit's operation.

Send JSON:

```json
{
  "request_id": "3",
  "work_id": "vad.1000",
  "action": "pause"
}
```

Response JSON:

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

`error::code` of 0 indicates successful execution.

## work

Resume the unit's operation.

Send JSON:

```json
{
  "request_id": "4",
  "work_id": "vad.1000",
  "action": "work"
}
```

Response JSON:

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

`error::code` of 0 indicates successful execution.

## exit

Exit the unit.

Send JSON:

```json
{
  "request_id": "5",
  "work_id": "vad.1000",
  "action": "exit"
}
```

Response JSON:

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

`error::code` of 0 indicates successful execution.

## taskinfo

Get the task list.

Send JSON:

```json
{
  "request_id": "2",
  "work_id": "vad",
  "action": "taskinfo"
}
```

Response JSON:

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

Get the task's running parameters.

```json
{
  "request_id": "2",
  "work_id": "vad.1000",
  "action": "taskinfo"
}
```

Response JSON:

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

> **Note: The work_id increases according to the order in which the units are initialized and is not a fixed index value.**
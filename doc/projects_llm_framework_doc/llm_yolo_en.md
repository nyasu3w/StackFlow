# llm-yolo

YOLO vision detection unit for providing image detection services. Various YOLO models can be selected.

## setup

Configuring the unit to work.

Send JSON:

```json
{
  "request_id": "2",
  "work_id": "yolo",
  "action": "setup",
  "object": "yolo.setup",
  "data": {
    "model": "yolo11n",
    "response_format": "yolo.box",
    "input": "yolo.jpg.base64",
    "enoutput": true
  }
}
```

- request_id: Reference for basic data explanation.
- work_id: Set to `yolo` during configuration.
- action: The method to call is `setup`.
- object: Data type being transmitted is `yolo.setup`.
- model: The model used is `yolo11n`.
- response_format: The returned result is `yolo.box`.
- input: The input is `yolo.jpg.base64`, which represents the data from the user input, encoded in jpg and base64.
- enoutput: Whether to enable user result output.

Response JSON:

```json
{
  "created": 1737596640,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "2",
  "work_id": "yolo.1001"
}
```

- created: Message creation time, Unix time.
- work_id: The successfully created work_id unit.

## exit

Unit exit.

Send JSON:

```json
{
  "request_id": "7",
  "work_id": "yolo.1001",
  "action": "exit"
}
```

Response JSON:

```json
{
  "created": 1737596871,
  "data": "None",
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "None",
  "request_id": "7",
  "work_id": "yolo.1001"
}
```

`error::code` of 0 means the execution was successful.

## taskinfo

Retrieve the task list.

Send JSON:

```json
{
  "request_id": "2",
  "work_id": "yolo",
  "action": "taskinfo"
}
```

Response JSON:

```json
{
  "created": 1737596668,
  "data": [
    "yolo.1001"
  ],
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "yolo.tasklist",
  "request_id": "2",
  "work_id": "yolo"
}
```

Retrieve task run parameters.

Send JSON:

```json
{
  "request_id": "2",
  "work_id": "yolo.1003",
  "action": "taskinfo"
}
```

Response JSON:

```json
{
  "created": 1737596698,
  "data": {
    "enoutput": true,
    "inputs": [
      "yolo.jpg.base64"
    ],
    "model": "yolo11n",
    "response_format": "yolo.box"
  },
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "yolo.taskinfo",
  "request_id": "2",
  "work_id": "yolo.1001"
}
```

> **Note: work_id increases according to the order of unit initialization, not a fixed index value.**  
# llm-camera

Video source unit for obtaining video streams from USB V4L2 video devices to internal channels.

## setup

Configuration unit operation.

Send JSON:

```json
{
  "request_id": "2",
  "work_id": "camera",
  "action": "setup",
  "object": "camera.setup",
  "data": {
    "response_format": "camera.raw",
    "input": "/dev/video0",
    "enoutput": false,
    "frame_width": 320,
    "frame_height": 320
  }
}
```

- request_id: Reference basic data explanation.
- work_id: When configuring the unit, it is `camera`.
- action: The method being called is `setup`.
- object: The data type being transmitted is `camera.setup`.
- response_format: The returned result is `camera.raw`, which is in YUV422 format.
- input: The name of the device being read.
- frame_width: The output video frame width.
- frame_height: The output video frame height.
- enoutput: Whether to enable the user result output. If camera images are not needed, do not enable this parameter, as
  the video stream will increase the communication load of the channel.

Response JSON:

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
  "work_id": "camera.1003"
}
```

- created: Message creation time, in Unix time.
- work_id: The work_id unit successfully created.

## exit

Exit the unit.

Send JSON:

```json
{
  "request_id": "7",
  "work_id": "camera.1003",
  "action": "exit"
}
```

Response JSON:

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
  "work_id": "camera.1003"
}
```

error::code of 0 indicates successful execution.

## taskinfo

Get the task list.

Send JSON:

```json
{
  "request_id": "2",
  "work_id": "camera",
  "action": "taskinfo"
}
```

Response JSON:

```json
{
  "created": 1731652311,
  "data": [
    "camera.1003"
  ],
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "camera.tasklist",
  "request_id": "2",
  "work_id": "camera"
}
```

Get task running parameters.

Send JSON:

```json
{
  "request_id": "2",
  "work_id": "camera.1003",
  "action": "taskinfo"
}
```

Response JSON:

```json
{
  "created": 1731652344,
  "data": {
    "enoutput": false,
    "response_format": "camera.raw",
    "input": "/dev/video0",
    "frame_width": 320,
    "frame_height": 320
  },
  "error": {
    "code": 0,
    "message": ""
  },
  "object": "camera.taskinfo",
  "request_id": "2",
  "work_id": "camera.1003"
}
```

> **Note: The work_id increases according to the order of unit initialization registration, not as a fixed index value.
**
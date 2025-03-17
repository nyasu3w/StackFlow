# llm-sys

StackFlow 的基本服务单元，对外提供串口和 TCP 外部信道和一部分系统功能服务，对内进行端口资源分配，和一个简单的内存数据库。

## 外部 API

- sys.ping：测试是否能够和 LLM 进行通信。
```json
{
    "request_id": "2",
    "work_id": "sys",
    "action": "ping"
}
```
- sys.lsmode：过去系统中存在的模型。
```json
{
    "request_id": "2",
    "work_id": "sys",
    "action": "lsmode"
}
```
- sys.bashexec：执行 bash 命令。
```json
{
    "request_id": "2",
    "work_id": "sys",
    "action": "bashexec",
    "object": "sys.utf-8.stream",
    "data" : {
        "index": 0,
        "delta": "ls",
        "finish": true
    }
}
```
- sys.hwinfo：获取 LLM 板载 cpu、内存、温度参数。
```json
{
    "request_id": "2",
    "work_id": "sys",
    "action": "hwinfo"
}
```
- sys.uartsetup：设置串口参数，单次生效。
```json
{
    "request_id": "2",
    "work_id": "sys",
    "action": "uartsetup",
    "object": "sys.uartsetup",
    "data" : {
        "baud": 115200,
        "data_bits": 8,
        "stop_bits": 1,
        "parity": 110
    }
}
```
- sys.reset：复位整个 LLM 框架的应用程序。
```json
{
    "request_id": "2",
    "work_id": "sys",
    "action": "reset"
}
```
- sys.reboot：重启系统。
```json
{
    "request_id": "2",
    "work_id": "sys",
    "action": "reboot"
}
```
- sys.version：获取 LLM 框架程序版本。
```json
{
    "request_id": "2",
    "work_id": "sys",
    "action": "version"
}
```

## 内部 API：

- sql_select：查寻小型内存 KV 数据库键值。
- register_unit：注册单元。
- release_unit：释放单元。
- sql_set：设定小型内存 KV 数据库键值。
- sql_unset：删除小型内存 KV 数据库键值。

/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "all.h"
#include <string>
#include "remote_action.h"
#include "pzmq.hpp"
#include "json.hpp"
#include "StackFlowUtil.h"

using namespace StackFlows;

int unit_call_timeout;

int remote_call(int com_id, const std::string &json_str)
{
    std::string work_id   = sample_json_str_get(json_str, "work_id");
    std::string work_unit = work_id.substr(0, work_id.find("."));
    std::string action    = sample_json_str_get(json_str, "action");
    char com_url[128];
    sprintf(com_url, zmq_c_format.c_str(), com_id);
    std::string send_data = "{\"zmq_com\":\"";
    send_data += std::string(com_url);
    send_data += "\",\"raw_data\":\"";
    send_data += sample_escapeString(json_str);
    send_data += "\"}";
    pzmq clent(work_unit);
    return clent.call_rpc_action(action, send_data, [](const std::string &val) {});
}

void remote_action_work()
{
    SAFE_READING(unit_call_timeout, int, "config_unit_call_timeout");
}

void remote_action_stop_work()
{
}

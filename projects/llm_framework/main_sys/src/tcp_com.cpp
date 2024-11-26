/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "all.h"
#include "hv/TcpServer.h"
#include <unordered_map>
// #include <functional>

#include <unistd.h>

#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "all.h"
#include "event_loop.h"
#include "zmq_bus.h"
#include "json.hpp"

using namespace hv;

std::atomic<int> counter_port(8000);
TcpServer srv;

class tcp_com : public zmq_bus_com {
private:
public:
    SocketChannelPtr channel;
    std::mutex tcp_server_mutex;
    tcp_com() : zmq_bus_com()
    {
    }

    void send_data(const std::string& data)
    {
        tcp_server_mutex.lock();
        if (exit_flage) channel->write(data);
        tcp_server_mutex.unlock();
    }
};

void onConnection(const SocketChannelPtr& channel)
{
    std::string peeraddr = channel->peeraddr();
    tcp_com* con_data;
    if (channel->isConnected()) {
        con_data          = new tcp_com();
        con_data->channel = channel;
        con_data->work(zmq_s_format, counter_port.fetch_add(1));
        if (counter_port.load() > 65535) counter_port.store(8000);
        channel->setContext(con_data);
    } else {
        con_data = (tcp_com*)channel->context();
        con_data->tcp_server_mutex.lock();
        con_data->stop();
        con_data->tcp_server_mutex.unlock();
        delete con_data;
    }
}

void onMessage(const SocketChannelPtr& channel, Buffer* buf)
{
    int len           = (int)buf->size();
    char* data        = (char*)buf->data();
    tcp_com* con_data = (tcp_com*)channel->context();
    con_data->tcp_server_mutex.lock();
    try {
        con_data->select_json_str(std::string(data, len),
                                  std::bind(&tcp_com::on_data, con_data, std::placeholders::_1));
    } catch (...) {
        std::string out_str;
        out_str += "{\"request_id\": \"0\",\"work_id\": \"sys\",\"created\": ";
        out_str += std::to_string(time(NULL));
        out_str += ",\"error\":{\"code\":-1, \"message\":\"reace reset\"}}";
        channel->write(out_str);
    }
    con_data->tcp_server_mutex.unlock();
}

void tcp_work()
{
    int listenport = 0;
    SAFE_READING(listenport, int, "config_tcp_server");

    int listenfd = srv.createsocket(listenport);
    if (listenfd < 0) {
        exit(-1);
    }
    printf("server listen on port %d, listenfd=%d ...\n", listenport, listenfd);
    srv.onConnection = onConnection;
    srv.onMessage    = onMessage;
    srv.setThreadNum(1);
    srv.start();
}
void tcp_stop_work()
{
    srv.stop();
}

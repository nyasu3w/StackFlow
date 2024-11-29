/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../../../SDK/components/utilities/include/sample_log.h"

int main_exit_flage = 0;
static void __sigint(int iSigNo)
{
    main_exit_flage = 1;
}


int main(int argc, char *argv[])
{
    // 打开默认摄像头
    cv::VideoCapture cap(0);

    // 检查摄像头是否成功打开
    if (!cap.isOpened()) {
        std::cerr << "无法打开摄像头" << std::endl;
        return -1;
    }

    cv::Mat frame;
    while (true) {
        // 读取摄像头的一帧
        cap >> frame;

        // 检查是否成功读取帧
        if (frame.empty()) {
            std::cerr << "无法接收帧 (可能是流的终止?)" << std::endl;
            break;
        }
        std::cout << "get ones  frame \n";
        // // 显示捕获的帧
        // cv::imshow("摄像头", frame);

        // // 按下'q'键退出
        // if (cv::waitKey(1) == 'q') {
        //     break;
        // }
    }

    // 释放摄像头并关闭所有窗口
    cap.release();
    cv::destroyAllWindows();

    return 0;
}

# StackFlow

<p align="center"><img src="https://static-cdn.m5stack.com/resource/public/assets/m5logo2022.svg" alt="basic" width="300" height="300"></p>


<p align="center">
  StackFlow 是一个简单、疾速、优雅的面向嵌入式开发者一站式 AI 服务基础设施项目，目的是让 Maker 和 Haker 们能够快速的在当前的嵌入式设备中获取到强大的 AI 加速能力。StackFlow 能够为各类人机交互设备注入智慧的灵魂。
</p>




## Table of Contents

* [特性](#特性)
* [Demo](#demo)
* [环境要求](#环境要求)
* [编译](#编译)
* [安装](#安装)
* [升级](#升级)
* [运行](#运行)
* [配置](#配置)
* [接口](#接口)
* [贡献](#贡献)


## 特性
<!-- ![](doc/assets/network.png) -->
* 分布式通信架构。每个单元都能够单独工作或者和其他单元配合工作。
* 多模型支持。包括但不限于语音识别、语音合成、图像识别、自然语言处理、LLM 大模型助手推理等。
* 数据内部流转。可根据需求配置不同单元协同工作，避免复杂的数据处理流程。
* 简单易用。通过标准 json 交换数据，快速实现 AI 服务。
* 离线化运行。无需联网即可实现本地 AI 服务。
* 多平台支持。包括但不限于 Module LLM、LLM630 Compute Kit 等。
* 灵活配置。所有单元均可完全配置运行参数，在相同的数据流处理情况下，随意更换模型和修改模型运行参数。
* 简单易用。开发者只需关注模型和硬件平台，无需关注底层通信和数据处理细节，即可快速实现 AI 服务。
* 高效稳定。通过 ZMQ 信道传输，数据传输效率高，延迟低，稳定性强。
* 开源免费。StackFlow 采用 MIT 许可证。
* 多语言支持。单元主体由 C++ 实现，性能极致优化，可扩展至多变成语言支持。（需要支持 ZMQ 编程）

StackFlow 还在不断优化和迭代，在框架更加完善的同时，会持续增加更多功能，敬请期待。

StackFlow 语音助手的主要工作模式：


开机后，KWS、ASR、LLM、TTS、AUDIO 被配置成协同工作状态。当 KWS 从 AUDIO 单元获取的音频中检测出关键词后，发出唤醒信号。此时，ASR 开始工作，识别 AUDIO 的音频数据，并将结果发布到自己的输出信道中。LLM 接收到 ASR 转换后的文本数据后，开始进行推理，并将结果发布到自己的输出信道中。TTS 接收到 LLM 的结果后，开始进行语音合成，合成完成后根据配置来播放合成好的音频数据。


## Demo
- [StackFlow 连续语音识别](./projects/llm_framework/README.md)
- [StackFlow LLM 大模型唤醒对话](./projects/llm_framework/README.md)
- [StackFlow TTS 语音合成播放](./projects/llm_framework/README.md)
- [StackFlow yolo 视觉检测](https://github.com/Abandon-ht/ModuleLLM_Development_Guide/tree/dev/ESP32/cpp)
- [StackFlow VLM 图片描述](https://github.com/Abandon-ht/ModuleLLM_Development_Guide/tree/dev/ESP32/cpp)

## 环境要求 ##
当前 StackFlow 的 AI 单元是建立在 AXERA 加速平台之上的，主要的芯片平台为 ax630c、ax650n。系统要求为 ubuntu。

## 编译 ##
StackFlow 主要的运行平台在嵌入式 linux 设备中，一般情况下请在主机 linux 设备中进行编译工作，编译工具链为 aarch64-none-linux-gnu。
```bash
# 安装 X86 交叉编译工具链
wget https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/linux/llm/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.gz
sudo tar zxvf gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.gz -C /opt

# 安装依赖
sudo apt install python3 python3-pip libffi-dev
pip3 install parse scons requests 

# 下载 StackFlow 源码
git clone https://github.com/m5stack/StackFlow.git
cd StackFlow
git submodule update --init
cd projects/llm_framework
scons distclean

# 编译。注意：编译时需要联网下载源码、二进制库等文件，请保持网络畅通。
scons -j22

# 打包 deb 包。注意：由于 LLM 的模型文件较大，打包 deb 包时，需要使用较大的磁盘空间，建议使用 128G 以上的磁盘空间。打包时会联网下载大量二进制文件，请注意流量消耗。
cd tools
python3 llm_pack.py
```

## 安装 ##
StackFlow 的程序和模型数据是分开的，程序安装完成后，需要单独下载模型数据，并配置到程序中。安装是先安装程序包，然后再安装模型包。
裸机环境安装(需要在 LLM 设备中执行下面命令):
```bash
# 首先安装动态库依赖
dpkg -i ./lib-llm_1.4-m5stack1_arm64.deb
# 然后安装 llm-sys 主单元
dpkg -i ./llm-sys_1.4-m5stack1_arm64.deb
# 安装其他 llm 单元
dpkg -i ./llm-xxx_1.4-m5stack1_arm64.deb
# 安装模型包
dpkg -i ./llm-xxx_1.4-m5stack1_arm64.deb
# 注意 lib-llm_1.4-m5stack1_arm64.deb 和 llm-sys_1.4-m5stack1_arm64.deb 的安装顺序，其他 llm 单元和模型包的安装顺序没有要求。
```

## 升级
升级时可单独升级 AI 单元，或者升级整个 StackFlow 框架。  
升级单个单元时可通过 SD 卡升级或者手动 `dpkg` 命令进行安装。需要注意的时在小版本的包中，可以单独安装升级包，但是大版本升级时，必须安装完所有的 llm 单元。
命令行升级包：
```bash
# 安装需要升级的 llm 单元
dpkg -i ./llm-xxx_1.4-m5stack1_arm64.deb
```
[设备自动升级安装](https://docs.m5stack.com/en/guide/llm/llm/image)
## 运行 ##
相关 AI 服务会在开机时自动运行，也可以通过手动命令启动。  
sys 单元运行状态查询：
```bash
systemctl status llm-sys
```
相关命令可参考 systemd 服务命令。

## 配置 ##
StackFlow 的配置分为两类，一类是单元工作参数配置，一类是模型工作参数配置。
两类配置文件均采用 json 格式，配置文件位于多个目录下，目录如下：
```
/opt/m5stack/data/models/
/opt/m5stack/share/
```
## 接口 ##
StackFlow 可通过 UART 和 TCP 端口进行访问。UART 端口的默认波特率为 115200，TCP 端口的默认端口为 10001。参数均可通过配置文件进行修改。

## 贡献

* 喜欢本项目请先打一颗星；
* 提 bug 请到 [issue 页面](https://github.com/m5stack/StackFlow/issues)；
* 要贡献代码，欢迎 fork 之后再提 pull request；

## Star 历史

[![Star History Chart](https://api.star-history.com/svg?repos=m5stack/StackFlow&type=Date)](https://star-history.com/#m5stack/StackFlow&Date)

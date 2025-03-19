#include "Tokenizer.hpp"

#include "sentencepiece_processor.h"
#include "builtin_pb/sentencepiece.pb.h"

#include "QwenTokenizer.hpp"

// #include "chatglm.h"

#include "httplib.h"
#include "json.hpp"

#include "sample_log.h"
#include "string_utility.hpp"
#include "memory_utils.hpp"

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstring>
#include <csignal>

class TokenizerLLaMa : public BaseTokenizer
{
protected:
    sentencepiece::SentencePieceProcessor sp;
    bool _b_bos, _b_eos;

private:
    /* data */
public:
    bool Init(std::string model_path, bool b_bos = true, bool b_eos = false) override
    {
        auto ret = sp.Load(model_path);
        if (!ret.ok())
        {
            ALOGE("%s", ret.error_message());
            return false;
        }

        this->_b_bos = b_bos;
        this->_b_eos = b_eos;
        return ret.ok();
    }

    bool Encode(std::string input, std::vector<int> &output, bool b_img_prompt = false) override
    {
        auto ret = sp.Encode(input, &output);
        if (!ret.ok())
        {
            ALOGE("%s", ret.error_message());
            return false;
        }
        if (_b_bos)
        {
            output.insert(output.begin(), sp.bos_id());
        }
        if (_b_eos)
        {
            output.push_back(sp.eos_id());
        }
        return true;
    }

    std::vector<int> Encode(std::string input, bool b_img_prompt = false) override
    {
        std::vector<int> output;
        Encode(input, output, b_img_prompt);
        return output;
    }

    std::string Decode(const std::vector<int> input) override
    {
        sentencepiece::SentencePieceText spt;
        sp.Decode(input, &spt);
        std::string out = spt.pieces()[0].piece();
        if (*(unsigned short *)out.data() == 38626)
        {
            return " " + spt.text();
        }
        else
        {
            return spt.text();
        }
    }

    int GetBosID() override
    {
        return sp.bos_id();
    }

    int GetEosID() override
    {
        return sp.eos_id();
    }
    std::string apply_chat_template() override
    {
        std::ostringstream oss_prompt;
        int messages_len = messages_.size();
        for(auto &message : messages_)
        {
            messages_len --;
            switch (message.first)
            {
            case ROLE_USER:
            {
                oss_prompt << "<|user|>\n" << message.second << "</s>";
            }
            break;
            case ROLE_SYSTEM:
            break;
            case ROLE_ASSISTANT:
            break;
            case ROLE_ASSISTANT_HELP:
            {
                if(messages_len == 0)
                {
                    oss_prompt << "<|assistant|>\n";
                }
            }
            break;
            default:
                break;
            }
        }
        return oss_prompt.str();
    }
};

class TokenizerMINICPM : public TokenizerLLaMa
{
public:
    std::string Decode(const std::vector<int> input) override
    {
        sentencepiece::SentencePieceText spt;
        sp.Decode(input, &spt);
        return spt.text();
    }
    std::string apply_chat_template() override
    {
        std::ostringstream oss_prompt;
        int messages_len = messages_.size();
        for(auto &message : messages_)
        {
            messages_len --;
            switch (message.first)
            {
            case ROLE_USER:
            {
                oss_prompt << "<用户>" << message.second;
            }
            break;
            case ROLE_SYSTEM:
            break;
            case ROLE_ASSISTANT:
            break;
            case ROLE_ASSISTANT_HELP:
            {
                if(messages_len == 0)
                {
                    oss_prompt << "<AI>";
                }
            }
            break;
            default:
                break;
            }
        }
        return oss_prompt.str();
    }
};

class TokenizerPhi3 : public BaseTokenizer
{
    sentencepiece::SentencePieceProcessor sp;
    bool _b_bos, _b_eos;

private:
    /* data */
public:
    bool Init(std::string model_path, bool b_bos = true, bool b_eos = false) override
    {
        auto ret = sp.Load(model_path);
        if (!ret.ok())
        {
            ALOGE("%s", ret.error_message());
            return false;
        }

        this->_b_bos = b_bos;
        this->_b_eos = b_eos;
        return ret.ok();
    }

    bool Encode(std::string input, std::vector<int> &output, bool b_img_prompt = false) override
    {
        auto ret = sp.Encode(input, &output);
        if (!ret.ok())
        {
            ALOGE("%s", ret.error_message());
            return false;
        }
        output.insert(output.begin(), 32010); //"<|user|>"
        output.push_back(32007);              //"<|end|>"
        output.push_back(32001);              //"<|assistant|>"
        if (_b_bos)
        {
            output.insert(output.begin(), sp.bos_id());
        }
        if (_b_eos)
        {
            output.push_back(sp.eos_id());
        }
        return true;
    }

    std::vector<int> Encode(std::string input, bool b_img_prompt = false) override
    {
        std::vector<int> output;
        Encode(input, output, b_img_prompt);
        return output;
    }

    std::string Decode(const std::vector<int> input) override
    {
        sentencepiece::SentencePieceText spt;
        sp.Decode(input, &spt);
        std::string out = spt.pieces()[0].piece();
        if (*(unsigned short *)out.data() == 38626)
        {
            return " " + spt.text();
        }
        else
        {
            return spt.text();
        }
    }

    int GetBosID() override
    {
        return sp.bos_id();
    }

    int GetEosID() override
    {
        return 32007;
    }

    bool isEnd(int id) override
    {
        return id == GetEosID() || id > 31999;
    }
    std::string apply_chat_template() override
    {
        std::ostringstream oss_prompt;
        int messages_len = messages_.size();
        for(auto &message : messages_)
        {
            messages_len --;
            switch (message.first)
            {
            case ROLE_USER:
            {
                oss_prompt << message.second;
            }
            break;
            case ROLE_SYSTEM:
            break;
            case ROLE_ASSISTANT:
            break;
            case ROLE_ASSISTANT_HELP:
            {
                if(messages_len == 0)
                {
                    oss_prompt << " ";
                }
            }
            break;
            default:
                break;
            }
        }
        return oss_prompt.str();
    }
};

class TokenizerQwen : public BaseTokenizer
{
    std::shared_ptr<QwenTokenizer> sp;
    bool _b_bos, _b_eos;

private:
    /* data */
public:
    bool Init(std::string model_path, bool b_bos = true, bool b_eos = false) override
    {
        if (!file_exist(model_path))
        {
            ALOGE("tokenizer model file(%s) not exist", model_path.c_str());
            return false;
        }

        sp.reset(new QwenTokenizer(model_path, QwenConfig()));

        this->_b_bos = b_bos;
        this->_b_eos = b_eos;
        return true;
    }

    bool Encode(std::string input, std::vector<int> &output, bool b_img_prompt = false) override
    {
        if (_b_bos)
        {
            // input += "<|im_start|>";
        }
        if (_b_eos)
        {
            input += "<|endoftext|>";
        }
        output = sp->encode(input, 1024);

        return true;
    }

    std::vector<int> Encode(std::string input, bool b_img_prompt = false) override
    {
        std::vector<int> output;
        Encode(input, output, b_img_prompt);
        return output;
    }

    std::string Decode(const std::vector<int> input) override
    {
        return sp->decode(input);
    }

    int GetBosID() override
    {
        return -1;
    }

    int GetEosID() override
    {
        return sp->eos_token_id;
    }
    std::string apply_chat_template() override
    {
        std::ostringstream oss_prompt;
        int messages_len = messages_.size();
        for(auto &message : messages_)
        {
            messages_len --;
            switch (message.first)
            {
            case ROLE_USER:
            {
                oss_prompt << "<|im_start|>user\n" << message.second << "<|im_end|>\n";
            }
            break;
            case ROLE_SYSTEM:
            {
                oss_prompt << "<|im_start|>system\n" << message.second << ".<|im_end|>\n";
            }
            break;
            case ROLE_ASSISTANT:
            {
                oss_prompt << "<|im_start|>assistant\n" << message.second << ".<|im_end|>\n";
            }
            break;
            case ROLE_ASSISTANT_HELP:
            {
                if(messages_len == 0)
                {
                    oss_prompt << "<|im_start|>assistant\n";
                }
            }
            break;
            default:
                break;
            }
        }
        return oss_prompt.str();
    }
};

// class TokenizerGLM3 : public BaseTokenizer
// {
//     std::shared_ptr<chatglm::ChatGLM3Tokenizer> sp;
//     bool _b_bos, _b_eos;

// private:
//     /* data */
// public:
//     bool Init(std::string model_path, bool b_bos = true, bool b_eos = false) override
//     {
//         if (!file_exist(model_path))
//         {
//             ALOGE("tokenizer model file(%s) not exist", model_path.c_str());
//             return false;
//         }
//         // std::vector<char> sp_model_data;
//         // read_file(model_path, sp_model_data);
//         // std::string_view serialized_model_proto(sp_model_data.data(), sp_model_data.size());

//         sp.reset(new chatglm::ChatGLM3Tokenizer(model_path));

//         this->_b_bos = b_bos;
//         this->_b_eos = b_eos;
//         return true;
//     }

//     bool Encode(std::string input, std::vector<int> &output) override
//     {
//         if (_b_bos)
//         {
//             // input += "<|im_start|>";
//         }
//         if (_b_eos)
//         {
//             // input += "<|endoftext|>";
//         }
//         output = sp->encode(input, 1024);

//         return true;
//     }

//     std::vector<int> Encode(std::string input) override
//     {
//         std::vector<int> output;
//         Encode(input, output);
//         return output;
//     }

//     std::string Decode(const std::vector<int> input) override
//     {
//         return sp->decode(input);
//     }

//     int GetBosID() override
//     {
//         return sp->sp.bos_id();
//     }

//     int GetEosID() override
//     {
//         return sp->sp.eos_id();
//     }
// };

class Tokenizer_Http : public BaseTokenizer
{
    std::shared_ptr<httplib::Client> cli;
    bool _b_bos, _b_eos;

    std::string base_url;

    int bos_id, eos_id;

private:
    /* data */
public:
    bool Init(std::string model_path = "http://localhost:8080", bool b_bos = true, bool b_eos = false) override
    {
        base_url = model_path;
        try
        {
            cli = std::make_shared<httplib::Client>(base_url);
            cli->set_connection_timeout(1);
            cli->set_read_timeout(1);
            cli->set_write_timeout(1);
            {
                auto ret = cli->Get("/bos_id");
                auto rep = ret.value();
                if (rep.status != 200)
                {
                    ALOGE("get bos_id failed, status: %d", rep.status);
                    return false;
                }
                nlohmann::json j = nlohmann::json::parse(rep.body);
                bos_id = j["bos_id"];
            }

            {
                auto ret = cli->Get("/eos_id");
                auto rep = ret.value();
                if (rep.status != 200)
                {
                    ALOGE("get eos_id failed, status: %d", rep.status);
                    return false;
                }
                nlohmann::json j = nlohmann::json::parse(rep.body);
                eos_id = j["eos_id"];
            }
            printf("bos_id: %d, eos_id: %d\n", bos_id, eos_id);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }

        this->_b_bos = b_bos;
        this->_b_eos = b_eos;
        return true;
    }

    bool Encode(std::string input, std::vector<int> &output, bool b_img_prompt = false) override
    {
        nlohmann::json j;
        j["text"] = input;
        j["img_prompt"] = b_img_prompt;
        auto ret = cli->Post("/encode", j.dump(), "application/json");
        auto rep = ret.value();
        if (rep.status != 200)
        {
            ALOGE("encode failed, status: %d", rep.status);
            return false;
        }
        nlohmann::json j2;
        try
        {
            j2 = nlohmann::json::parse(rep.body);
        }
        catch (const std::exception &e)
        {
            ALOGE("json parse failed: %s", e.what());
            ALOGE("%s", rep.body.c_str());
            return false;
        }

        std::vector<int> out = j2["token_ids"];
        output = out;
        // output = sp->encode(input, 1024);
        if (_b_bos)
        {
            output.insert(output.begin(), bos_id);
        }
        if (_b_eos)
        {
            output.push_back(eos_id);
        }

        return true;
    }

    std::vector<int> Encode(std::string input, bool b_img_prompt = false) override
    {
        std::vector<int> output;
        Encode(input, output, b_img_prompt);
        return output;
    }

    std::string Decode(const std::vector<int> input) override
    {
        int cnt = 2;
        std::string out_str = "";
        while (cnt--)
        {
            nlohmann::json j;
            j["token_ids"] = input;
            auto ret = cli->Post("/decode", j.dump(), "application/json");
            auto rep = ret.value();
            if (rep.status != 200)
            {
                ALOGE("decode failed, status: %d, try again", rep.status);
                ALOGE("%s", rep.body.c_str());
                usleep(1000 * 1000);
                continue;
            }
            try
            {
                nlohmann::json j2 = nlohmann::json::parse(rep.body);
                out_str = j2["text"];
                break;
            }
            catch (const std::exception &e)
            {
                ALOGE("json parse failed: %s, try again", e.what());
                ALOGE("%s", rep.body.c_str());
                usleep(1000 * 1000);
                continue;
            }
        }
        return out_str;
    }

    int GetBosID() override
    {
        return bos_id;
    }

    int GetEosID() override
    {
        return eos_id;
    }
    std::string apply_chat_template() override
    {
        std::ostringstream oss_prompt;
        int messages_len = messages_.size();
        for(auto &message : messages_)
        {
            messages_len --;
            switch (message.first)
            {
            case ROLE_USER:
            {
                oss_prompt << message.second ;
            }
            break;
            case ROLE_SYSTEM:
            {
            }
            break;
            case ROLE_ASSISTANT:
            {
            }
            break;
            case ROLE_ASSISTANT_HELP:
            {
            }
            break;
            default:
                break;
            }
        }
        return oss_prompt.str();
    }
};


class ResultObj
{
private:
    /* data */
public:
    bool success;
    nlohmann::json result;
    ResultObj(bool _success, const nlohmann::json &_result){
        success = _success;
        result = _result;
    }
};

class Tokenizer_Auto : public BaseTokenizer
{
    bool _b_bos, _b_eos;
    std::string base_url;
    int bos_id, eos_id;
    pid_t pid = 0;
    int pipe_c2p[2] = {0}; // C++ -> Python
    int pipe_p2c[2] = {0}; // Python -> C++
private:
    ResultObj readPython(int id, int timeout = 10000)
    {
        std::string message;
        message.reserve(1056);
        static char buffer[1056];
        while (timeout > 0) {
            ssize_t bytesRead = read(pipe_p2c[0], buffer, sizeof(buffer) - 1);
            if (bytesRead > 0) {
                for(int i = 0; i < bytesRead; i++)
                {
                    message += buffer[i];
                    if(buffer[i] == '\n')
                    {
                        ALOGE("readPython:%s", message.c_str());
                        int pos = message.size() - 12;
                        if((message.size() > 12)&&(message.substr(pos) == "rpccontinue\n"))
                        {
                            message.erase(pos);
                            continue;
                        }
                        nlohmann::json j2;
                        try
                        {
                            j2 = nlohmann::json::parse(message);
                            if(j2["id"].is_number_integer() && j2["id"] == id)
                            {
                                return ResultObj(true, j2["result"]);
                            }
                            else if(j2["id"].is_null())
                            {
                                return ResultObj(false, j2);
                            }
                        }
                        catch (...)
                        {
                            message = "";
                            continue;
                        }
                    }
                }
            }
            else
            {
                break;
            }
            usleep(10000);
            timeout -= 10;
        }
        return ResultObj(false, nullptr);
    }

    ResultObj callPython(nlohmann::json &rpcobj)
    {
        static int id = 0;
        rpcobj["jsonrpc"] = "2.0";
        rpcobj["id"] = ++id;
        std::string _rawdata = rpcobj.dump();
        _rawdata += "\n";
        std::string rawdata;
        rawdata.reserve(_rawdata.size());
        for (int i = 0; i < _rawdata.length(); i++)
        {
            if((_rawdata.length() - i > 4)&&(_rawdata[i] == '\\')&&(_rawdata[i + 1] == '\\')&&(_rawdata[i + 2] == 'u'))
            {
                rawdata += '\\';
                rawdata += 'u';
                i += 2;
            }
            else
            {
                rawdata += _rawdata[i];
            }
        }
        const char* delstr = "\"DELSELF\":0";
        size_t pos = rawdata.find(delstr);
        if(pos != std::string::npos)
        {
            rawdata.erase(pos, strlen(delstr));
        }
        for(;;)
        {
            if(rawdata.size() > 1024)
            {
                auto part_data = rawdata.substr(0, 1024);
                part_data += "rpccontinue\n";
                ALOGE("callPython write:%s", part_data.c_str());
                write(pipe_c2p[1], part_data.c_str(), part_data.size());
                rawdata.erase(0, 1024);
                continue;
            }
            ALOGE("callPython write:%s", rawdata.c_str());
            write(pipe_c2p[1], rawdata.c_str(), rawdata.size());
            break;
        }
        return readPython(id);
    }

    void setNonBlocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1) {
            perror("fcntl F_GETFL");
            exit(EXIT_FAILURE);
        }
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            perror("fcntl F_SETFL");
            exit(EXIT_FAILURE);
        }
    }
public:
    ~Tokenizer_Auto()
    {
        if(pid > 0)
        {
            kill(pid, SIGTERM);
            waitpid(pid, nullptr, 0);
        }
    }
    bool Init(std::string model_path = "tokenizer", bool b_bos = true, bool b_eos = false) override
    {
        ALOGE("Tokenizer_Auto model_path:%s", model_path.c_str());
        if (pipe(pipe_c2p) == -1 || pipe(pipe_p2c) == -1) {
            perror("pipe");
            return false;
        }
        pid = fork();
        if (pid == -1) {
            perror("fork");
            return false;
        }
        if (pid == 0) {
            // 子进程（Python）
            // 子进程关闭不必要的管道端
            close(pipe_c2p[1]); // 关闭写端
            close(pipe_p2c[0]); // 关闭读端
            // 将管道端重定向到标准输入输出
            dup2(pipe_c2p[0], STDIN_FILENO); // 将C++写端重定向为Python的标准输入
            dup2(pipe_p2c[1], STDOUT_FILENO); // 将Python的标准输出重定向为C++的读端
            // 关闭重定向后的管道端
            close(pipe_c2p[0]);
            close(pipe_p2c[1]);
            // 执行Python脚本
            execlp("/usr/bin/python3", "python3", "/opt/m5stack/share/tokenizer_auto.py", "--model_id", model_path.c_str(), nullptr);
            perror("execlp");
            exit(EXIT_FAILURE);
        } 
        // 父进程（C++）
        // 父进程关闭不必要的管道端
        close(pipe_c2p[0]); // 关闭读端
        close(pipe_p2c[1]); // 关闭写端
        auto ret = readPython(0, 15000);
        if(ret.success)
        {
            bos_id = ret.result["bos_id"].is_number_integer() ? (int)ret.result["bos_id"] : 0;
            eos_id = ret.result["eos_id"].is_number_integer() ? (int)ret.result["eos_id"] : 0;
            this->_b_bos = b_bos;
            this->_b_eos = b_eos;
        }
        return ret.success;
    }

    bool Encode(std::string input, std::vector<int> &output, bool b_img_prompt = false) override
    {

        nlohmann::json rpcobj;
        rpcobj["method"] = "encode";
        rpcobj["params"] = nlohmann::json::array({nlohmann::json::array({input}), {{"DELSELF", 0}}});
        auto ret = callPython(rpcobj);
        if(ret.success)
        {
            if (ret.result.is_array()) {
                output = ret.result.get<std::vector<int>>();
            } else {
                std::cout << "result 键不存在或不是一个数组！" << std::endl;
            }
        }
        return ret.success;
    }

    std::vector<int> Encode(std::string input, bool b_img_prompt = false) override
    {
        std::vector<int> output;
        Encode(input, output, b_img_prompt);
        return output;
    }

    std::string Decode(const std::vector<int> input) override
    {
        nlohmann::json rpcobj;
        rpcobj["method"] = "decode";
        rpcobj["params"] = nlohmann::json::array({nlohmann::json::array({input}), {{"DELSELF", 0}}});
        auto ret = callPython(rpcobj);
        if(ret.success)
        {
            return ret.result.get<std::string>();
        }
        return "";
    }
    
    int GetBosID() override
    {
        return bos_id;
    }

    int GetEosID() override
    {
        return eos_id;
    }
    std::string apply_chat_template() override
    {
        // std::list<std::pair<std::string, std::string>> messages;        
        // auto ret = callPython("apply_chat_template", R"([[[{"role":"user","content":"hello"}]],{"tokenize":false,"add_generation_prompt":true}])");
        std::list<nlohmann::json> messages_list;
        // nlohmann::json messages_list = nlohmann::json::array();
        for(auto &message : messages_)
        {
            nlohmann::json msg;
            switch (message.first)
            {
            case ROLE_USER:
            {
                // {"role": "user", "content": prompt}
                msg["role"] = "user";
                msg["content"] = (std::string)message.second;
                messages_list.push_back(msg);
            }
            break;
            case ROLE_SYSTEM:
            {
                msg["role"] = "system";
                msg["content"] = message.second;
                messages_list.push_back(msg);
            }
            break;
            case ROLE_ASSISTANT:
            {
                msg["role"] = "assistant";
                msg["content"] = message.second;
                messages_list.push_back(msg);
            }
            break;
            case ROLE_ASSISTANT_HELP:
            {
            }
            break;
            default:
                break;
            }
        }        
        nlohmann::json rpcobj;
        rpcobj["method"] = "apply_chat_template";
        rpcobj["params"] = nlohmann::json::array({nlohmann::json::array({messages_list}), {{"tokenize", false}, {"add_generation_prompt", true}}});
        auto ret = callPython(rpcobj);
        if(ret.success)
        {
            std::string out_str = (std::string)ret.result;
            ALOGE("out_str:%s", out_str.c_str());
            return out_str;
        }
        return "";
    }
};

std::shared_ptr<BaseTokenizer> CreateTokenizer(TokenizerType type)
{
    switch (type)
    {
    case TKT_LLaMa:
        return std::make_shared<TokenizerLLaMa>();
    case TKT_MINICPM:
        return std::make_shared<TokenizerMINICPM>();
    case TKT_HTTP:
        return std::make_shared<Tokenizer_Http>();
    case TKT_Qwen:
        return std::make_shared<TokenizerQwen>();
    case TKT_Phi3:
        return std::make_shared<TokenizerPhi3>();
    case TKT_AUTO:
        return std::make_shared<Tokenizer_Auto>();
    default:
        return nullptr;
    }
}






// {"id":32,"jsonrpc":"2.0","method":"decode","params":[[[15469,1,13352,369,330,471,16488,11229,497,892,374,264,8870,315,6366,8038,429,23497,389,6825,24514,12645,429,646,2736,9079,429,11136,1373,3738,11229,11,1741,438,6832,504,821,323,3259,11181,13,15235,17601,279,4401,315,25185,11,4119,11,323,5942,429,646,23643,821,11,3960,504,3139,11,323,1281,11181,3118,389,429,6540,13,15235,702,1657,8357,304,5257,5043,1741,438,18478,11,17017,11,17903,11,323,803,13]],{}]}



// {"id":404,"jsonrpc":"2.0","method":"decode","params":[[[131070,796,15371,379,655,10919,78,198,6582,40887,1594,11825,20172,1531,650,33900,29040,1187,64466,409,1187,308,3760,1395,92433,655,65484,379,511,5165,64,662,5093,42838,645,409,18299,78,11,1709,1320,55611,264,49352,1594,10919,78,13,38676,18299,78,511,93676,64466,379,15466,662,733,4942,88026,11,1395,269,5039,78,655,65484,28526,74783,264,5141,24743,1603,1594,11825,20172,92001,399,78,13,55708,64466,1395,92433,1187,82309,409,5093,7473,3655,409,15045,1794,67116,11,6368,4883,4154,655,10919,78,379,32502,4883,264,5141,24743,1603,1594,11825,20172,13,4929,64466,1395,92433,913,83444,409,43892,733,4942,379,1531,1187,140100,4154,1187,1709,655,11825,20172,92001,399,78,20673,23191,3828,264,465,65251,11,13526,1709,902,41598,1531,9434,13,17632,47602,11,17669,11825,52430,24743,1603,92001,399,300,1709,40504,1420,10918,2098,20410,379,511,40504,82911,261,13,19801,57200,11,5141,24743,1603,1594,11825,20172,92001,399,78,409,48976,297,655,326,6304,409,619,6654,79,2015,4438,22506,2098,20410,379,511,40504,82911,261,662,1187,64466,1594,11825,20172,92001,399,78,13,87483,11,5141,24743,1603,409,48976,379,619,6654,79,2015,4438,22506,264,465,65251,300,90892,264,913,64466,24245,64,1709,20673,29570,391,277,1187,39036,409,2478,9282,18244,13,2308,1508,4706,68,11,58366,5141,24743,1603,92001,399,300,40504,1420,10918,2098,20410,379,264,465,65251,300,11,902,30101,4438,14197,24245,300,7953,5141,24743,1603,409,48976,297,619,6654,79,2015,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,35797,409,1187,24949,662,655,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187]],{}]}
// {"id":404,"jsonrpc":"2.0","method":"decode","params":[[[131070,796,15371,379,655,10919,78,198,6582,40887,1594,11825,20172,1531,650,33900,29040,1187,64466,409,1187,308,3760,1395,92433,655,65484,379,511,5165,64,662,5093,42838,645,409,18299,78,11,1709,1320,55611,264,49352,1594,10919,78,13,38676,18299,78,511,93676,64466,379,15466,662,733,4942,88026,11,1395,269,5039,78,655,65484,28526,74783,264,5141,24743,1603,1594,11825,20172,92001,399,78,13,55708,64466,1395,92433,1187,82309,409,5093,7473,3655,409,15045,1794,67116,11,6368,4883,4154,655,10919,78,379,32502,4883,264,5141,24743,1603,1594,11825,20172,13,4929,64466,1395,92433,913,83444,409,43892,733,4942,379,1531,1187,140100,4154,1187,1709,655,11825,20172,92001,399,78,20673,23191,3828,264,465,65251,11,13526,1709,902,41598,1531,9434,13,17632,47602,11,17669,11825,52430,24743,1603,92001,399,300,1709,40504,1420,10918,2098,20410,379,511,40504,82911,261,13,19801,57200,11,5141,24743,1603,1594,11825,20172,92001,399,78,409,48976,297,655,326,6304,409,619,6654,79,2015,4438,22506,2098,20410,379,511,40504,82911,261,662,1187,64466,1594,11825,20172,92001,399,78,13,87483,11,5141,24743,1603,409,48976,379,619,6654,79,2015,4438,22506,264,465,65251,300,90892,264,913,64466,24245,64,1709,20673,29570,391,277,1187,39036,409,2478,9282,18244,13,2308,1508,4706,68,11,58366,5141,24743,1603,92001,399,300,40504,1420,10918,2098,20410,379,264,465,65251,300,11,902,30101,4438,14197,24245,300,7953,5141,24743,1603,409,48976,297,619,6654,79,2015,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,35797,409,1187,24949,662,655,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950,6743,650,69157,16587,662,54896,2478,11763,3630,16282,6654,276,390,1187,64466,1594,11825,20172,92001,399,78,13,4929,64466,409,1187,308,3760,1395,92433,655,65484,379,1320,55611,264,49352,1594,10919,78,11,6368,4883,4154,655,11825,20172,92001,399,78,28526,74783,264,5141,24743,1603,409,1187,22325,13886,379,38405,56295,966,6494,267,288,13,2925,592,27899,11,1187,64466,409,1187,308,3760,1531,5093,19538,15811,1594,10919,78,379,46950]],{}]}
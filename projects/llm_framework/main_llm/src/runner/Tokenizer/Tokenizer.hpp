#pragma once
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <utility>
#include <iostream>
enum TokenizerType
{
    TKT_LLaMa,
    TKT_Qwen,
    TKT_HTTP,
    TKT_Phi3,
    TKT_MINICPM,
    TKT_AUTO,
    TKT_END
};
enum TokenizeRole{
    ROLE_USER,//用户输入
    ROLE_SYSTEM,//提示词
    ROLE_ASSISTANT,//助手回复
    ROLE_ASSISTANT_HELP// 询问句
};

class BaseTokenizer
{
public:
    std::list<std::pair<enum TokenizeRole, std::string>> messages_;
    void messages_clean() {messages_.clear();};
public:
    virtual bool Init(std::string model_path, bool b_bos = true, bool b_eos = false) = 0;
    virtual bool Encode(std::string input, std::vector<int> &output, bool b_img_prompt = false) = 0;
    virtual std::vector<int> Encode(std::string input, bool b_img_prompt = false) = 0;
    virtual std::string Decode(const std::vector<int> input) = 0;
    virtual int GetBosID() = 0;
    virtual int GetEosID() = 0;
    virtual std::string apply_chat_template() = 0;
    virtual std::string messages_complete(enum TokenizeRole role, const std::string &content = ""){
        messages_.push_back(std::make_pair(role, content));
        // std::cout << "messages_complete role:" << role << "content:" << content << std::endl;
        if(ROLE_ASSISTANT_HELP == role)
            return apply_chat_template();
        else
            return "";
    }
    virtual bool isEnd(int id) { return id == GetEosID(); }
};

std::shared_ptr<BaseTokenizer> CreateTokenizer(TokenizerType type);
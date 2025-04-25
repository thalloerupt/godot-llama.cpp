#include <vector>
#include <string>
#include <ctime>
#include <iostream>
#include <map>
#include "llama.h"
#include <string.h>
#include <functional>
#pragma once

// 对话元数据结构
struct ConversationMeta {
    std::string title;
    std::time_t created_time;
    std::time_t last_used_time;
    size_t token_count;
};

class MultiConversationManager {
private:
    llama_model* model;
    std::map<int, llama_context*> contexts;
    std::map<int, std::vector<llama_token>> histories;
    std::map<int, ConversationMeta> metas;
    std::map<int, std::vector<llama_chat_message>> session_messages;
    std::map<int, int> session_index;
    int next_id;
    size_t max_history_tokens;
    const llama_vocab * vocab;
    llama_sampler * smpl;



    std::string detokenize(int conv_id, const std::vector<llama_token>& tokens) {
        std::string result;
        for (auto token : tokens) {
            char buf[256];
            
            int n = llama_token_to_piece(vocab, token, buf, sizeof(buf), 0, true);
            if (n < 0) {
                GGML_ABORT("failed to convert token to piece\n");
            }
            std::string piece(buf, n);
            result += piece;
        }
        return result;
    }

    void trimHistory(int conv_id) {
        auto& history = histories[conv_id];
        if (history.size() > max_history_tokens) {
            size_t to_remove = history.size() - max_history_tokens;
            history.erase(history.begin(), history.begin() + to_remove);
            metas[conv_id].token_count = history.size();
        }
    }


    std::string create_prompt(std::string input,std::vector<llama_chat_message> messages,std::vector<char> formatted,int prev_len){
        const char * tmpl = llama_model_chat_template(model, /* name */ nullptr);
        // add the user input to the message list and format it
        messages.push_back({"user", strdup(input.c_str())});
        int new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, formatted.data(), formatted.size());
        if (new_len > (int)formatted.size()) {
            formatted.resize(new_len);
            new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, formatted.data(), formatted.size());
        }
        if (new_len < 0) {
            fprintf(stderr, "failed to apply the chat template\n");
            return "";
        }

        // remove previous messages to obtain the prompt to generate the response
        std::string prompt(formatted.begin() + prev_len, formatted.begin() + new_len);
        return prompt;

    }


public:
    MultiConversationManager(llama_model* model, 
                           const llama_vocab* _vocab,
                           size_t max_history = 2048
                           
                           )
        : model(model),next_id(0),vocab(_vocab),max_history_tokens(max_history) {}

    ~MultiConversationManager() {
        for (auto& pair : contexts) {
            llama_free(pair.second);
        }
    }

    // 创建新对话
    int createConversation(const std::string& title = "") {
        int conv_id = next_id++;
        llama_context_params ctx_params = llama_context_default_params();
        ctx_params.n_ctx = 2048;
        ctx_params.n_batch = 512;
        llama_context * ctx = llama_init_from_model(model, ctx_params);
        // initialize the sampler
        smpl = llama_sampler_chain_init(llama_sampler_chain_default_params());
        llama_sampler_chain_add(smpl, llama_sampler_init_min_p(0.05f, 1));
        llama_sampler_chain_add(smpl, llama_sampler_init_temp(0.8f));
        llama_sampler_chain_add(smpl, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

        std::vector<llama_chat_message> messages;
        session_index.insert({conv_id,0});
        session_messages.insert({conv_id,messages});


        contexts.insert({conv_id, ctx});
        histories.insert({conv_id, {std::vector<llama_token>()}});
        metas[conv_id] = {
            title.empty() ? "对话" + std::to_string(conv_id) : title,
            std::time(nullptr),
            std::time(nullptr),
            0
        };
        return conv_id;
    }

    // 删除对话
    bool removeConversation(int conv_id) {
        auto it = contexts.find(conv_id);
        if (it == contexts.end()) return false;

        llama_free(it->second);
        contexts.erase(it);
        histories.erase(conv_id);
        metas.erase(conv_id);
        return true;
    }



    

    // 生成响应
    std::string generate (int conv_id,const std::string & input,std::function<void(const std::string&)> callback) {
        std::string response;
        std::string lineBuffer;         
        llama_context * ctx = contexts[conv_id];
        int index = session_index[conv_id];

        const bool is_first = true;

        std::vector<char> formatted(llama_n_ctx(ctx));


        std::string prompt = create_prompt(input,session_messages[conv_id],formatted,index);

        

        // tokenize the prompt
        const int n_prompt_tokens = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), NULL, 0, is_first, true);
        std::vector<llama_token> prompt_tokens(n_prompt_tokens);
        if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), is_first, true) < 0) {
            GGML_ABORT("failed to tokenize the prompt\n");
        }

        // prepare a batch for the prompt
        llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());
        llama_token new_token_id;
        while (true) {
            // check if we have enough space in the context to evaluate this batch
            int n_ctx = llama_n_ctx(ctx);
            int n_ctx_used = llama_kv_self_used_cells(ctx);
            if (n_ctx_used + batch.n_tokens > n_ctx) {
                printf("\033[0m\n");
                fprintf(stderr, "context size exceeded\n");
                exit(0);
            }

            if (llama_decode(ctx, batch)) {
                GGML_ABORT("failed to decode\n");
            }

            // sample the next token
            new_token_id = llama_sampler_sample(smpl, ctx, -1);

            // is it an end of generation?
            if (llama_vocab_is_eog(vocab, new_token_id)) {
                break;
            }

            // convert the token to a string, print it and add it to the response
            char buf[256];
            
            int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
            if (n < 0) {
                GGML_ABORT("failed to convert token to piece\n");
            }
            std::string piece(buf, n);
            fflush(stdout);
            response += piece;
            lineBuffer += piece; // 将新生成的 token 添加到缓冲区
            size_t pos;
            while ((pos = lineBuffer.find('\n')) != std::string::npos) {
                std::string line = lineBuffer.substr(0, pos);
                callback(line);
                lineBuffer.erase(0, pos + 1); // +1 去掉 \n 本身
            }
            // prepare the next batch with the sampled token
            batch = llama_batch_get_one(&new_token_id, 1);
        }

        return response;
    };



    // 获取对话历史
    std::string getConversationHistory(int conv_id) {
        if (histories.find(conv_id) == histories.end()) {
            return "";
        }
        return detokenize(conv_id, histories[conv_id]);
    }

    // 获取对话元数据
    ConversationMeta getConversationMeta(int conv_id) {
        if (metas.find(conv_id) == metas.end()) {
            return {"", 0, 0, 0};
        }
        return metas[conv_id];
    }

    // 获取所有对话ID
    std::vector<int> getAllConversationIds() {
        std::vector<int> ids;
        for (const auto& pair : contexts) {
            ids.push_back(pair.first);
        }
        return ids;
    }

    // 清除对话历史但保留上下文
    void clearHistory(int conv_id) {
        if (histories.find(conv_id) != histories.end()) {
            histories[conv_id].clear();
            metas[conv_id].token_count = 0;
            metas[conv_id].last_used_time = std::time(nullptr);
        }
    }
};
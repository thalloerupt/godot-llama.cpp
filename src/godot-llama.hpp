#include <vector>
#include <string>
#include <ctime>
#include <map>
#include "llama.h"

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
    llama_context_params ctx_params;
    std::map<int, llama_context*> contexts;
    std::map<int, std::vector<llama_token>> histories;
    std::map<int, ConversationMeta> metas;
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

public:
    MultiConversationManager(llama_model* model, 
                           const llama_context_params& params,
                           const llama_vocab* _vocab,
                           llama_sampler * smpl,
                           size_t max_history = 2048
                           
                           )
        : model(model), ctx_params(params), next_id(0),vocab(_vocab),smpl(smpl) ,max_history_tokens(max_history) {}

    ~MultiConversationManager() {
        for (auto& pair : contexts) {
            llama_free(pair.second);
        }
    }

    // 创建新对话
    int createConversation(const std::string& title = "") {
        int conv_id = next_id++;
        contexts[conv_id] = llama_new_context_with_model(model, ctx_params);
        histories[conv_id] = std::vector<llama_token>();
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
    std::string generate (int conv_id,const std::string & prompt) {
        std::string response;
        llama_context * ctx = contexts[conv_id];

        const bool is_first = llama_kv_self_used_cells(ctx) == 0;

        

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
            printf("%s", piece.c_str());
            fflush(stdout);
            response += piece;

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
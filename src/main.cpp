#include "godot-llama.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char ** argv) {
       // 初始化llama.cpp
    llama_backend_init();

    // 加载模型
    llama_model_params model_params = llama_model_default_params();
    llama_model* model = llama_load_model_from_file("/home/luo/下载/Qwen2.5-0.5B-Instruct.Q8_0.gguf" , model_params);
    const llama_vocab * vocab = llama_model_get_vocab(model);
    if (!model) {
        std::cerr << "无法加载模型" << std::endl;
        return 1;
    }



    // 设置上下文参数
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = 2048;
    ctx_params.n_threads = 4;
    ctx_params.n_threads_batch = 4;

    // initialize the sampler
    llama_sampler * smpl = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(smpl, llama_sampler_init_min_p(0.05f, 1));
    llama_sampler_chain_add(smpl, llama_sampler_init_temp(0.8f));
    llama_sampler_chain_add(smpl, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));


    // 创建对话管理器
    MultiConversationManager manager(model, ctx_params, vocab,smpl,1024);

    // 创建两个对话
    int tech_conv = manager.createConversation("技术讨论");
    int story_conv = manager.createConversation("创意写作");

    // 在技术对话中交互
    std::string tech_response = manager.generate(tech_conv, "如何优化C++代码的内存使用?,不超过15字");
    std::cout << "技术对话响应:\n" << tech_response << "\n\n";

    // 在故事对话中交互
    std::string story_response = manager.generate(story_conv, "写一个关于人工智能的短故事开头，设定在未来的火星殖民地");
    std::cout << "故事对话响应:\n" << story_response << "\n\n";

    // 继续技术对话
    tech_response = manager.generate(tech_conv, "能给出具体的代码示例吗?");
    std::cout << "技术对话响应:\n" << tech_response << "\n\n";

    // 显示对话信息
    auto tech_meta = manager.getConversationMeta(tech_conv);
    std::cout << "技术对话信息:\n"
              << "标题: " << tech_meta.title << "\n"
              << "创建时间: " << std::ctime(&tech_meta.created_time)
              << "Token数: " << tech_meta.token_count << "\n\n";

    // 清理
    llama_free_model(model);
    llama_backend_free();

    return 0;
}

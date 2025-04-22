#include "godot-llama.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char ** argv) {
        std::string model_path;

        for (int i = 1; i < argc; i++) {
        try {
            if (strcmp(argv[i], "-m") == 0) {
                if (i + 1 < argc) {
                    model_path = argv[++i];
                } else {
                    printf("Please input model path.");
                    return 1;
                }
            }
        }catch (std::exception & e) {
            fprintf(stderr, "error: %s\n", e.what());
            return 1;
        }
        }



       // 初始化llama.cpp
    ggml_backend_load_all();

    // 加载模型
    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = 99;
    llama_model* model = llama_model_load_from_file(model_path.c_str() , model_params);
    const llama_vocab * vocab = llama_model_get_vocab(model);
    if (!model) {
        std::cerr << "无法加载模型" << std::endl;
        return 1;
    }




    // 创建对话管理器
    MultiConversationManager manager(model, vocab,2048);

    // 创建两个对话
    int tech_conv = manager.createConversation("Introduce yourself");
    int story_conv = manager.createConversation("创意写作");



    
    std::cout << "\033[32m技术对话响应:\033[0m\n" << std::endl;

    // 在技术对话中交互
    std::string tech_response = manager.generate(tech_conv, "hello");

    std::cout << "\n\n\033[32m故事对话响应:\033[0m\n"  << "\n";
    // 在故事对话中交互
    std::string story_response = manager.generate(story_conv, "写一个关于人工智能的短故事开头，设定在未来的火星殖民地");
    
    std::cout << "\n\n\033[32m技术对话响应:\033[0m\n" << "\n";
    // 继续技术对话
    tech_response = manager.generate(tech_conv, "能给出具体的代码示例吗?");
    

    // 显示对话信息
    auto tech_meta = manager.getConversationMeta(tech_conv);
    std::cout << "\n\n技术对话信息:\n"
              << "标题: " << tech_meta.title << "\n"
              << "创建时间: " << std::ctime(&tech_meta.created_time)
              << "Token数: " << tech_meta.token_count << "\n\n";

    // 清理
    llama_free_model(model);
    llama_backend_free();

    return 0;
}

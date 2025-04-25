#include "gdllamaholder.h"
#include "conversion.hpp"
#include "godot-llama.hpp"

#include <cstring>
#include <string>

#include <godot_cpp/core/class_db.hpp>
#include <string>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;

void GDLlamaHolder::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlamaHolder::get_model_path);
	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlamaHolder::set_model_path);
    ClassDB::add_property("GDLlamaHolder", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");


	ClassDB::bind_method(D_METHOD("initialize"), &GDLlamaHolder::initialize);


}

GDLlamaHolder::GDLlamaHolder() {

}

GDLlamaHolder::~GDLlamaHolder() {
	// Add your cleanup here.
}

void GDLlamaHolder::_ready() {
	// Called when the node is added to the scene tree.
	// You can initialize your model here if needed.
	
}

void GDLlamaHolder::_process(double delta) {
	

}


String GDLlamaHolder::get_model_path() const {
	
    return string_std_to_gd(model_path);
}

void GDLlamaHolder::set_model_path(const String p_model_path) {
	print_line(p_model_path);
    model_path = string_gd_to_std(p_model_path.trim_prefix(String("res://")));
}

void GDLlamaHolder::initialize() {
	// Initialize the llama model here using the model_path.
	// Add your initialization code here.
	ggml_backend_load_all();
    

    // 加载模型
    llama_model_params model_params = llama_model_default_params();
    
   
    
    model_params.n_gpu_layers = 99;
    
	llama_model* model = llama_model_load_from_file(model_path.c_str(), model_params);

	const llama_vocab * vocab = llama_model_get_vocab(model);
    if (!model) {
        print_error("Can't load model:" + string_std_to_gd(model_path));
        return ;
    }

	    // 创建对话管理器
	MultiConversationManager _manager(model, vocab,2048);
	
	_managers = std::make_unique<MultiConversationManager>(model, vocab, 2048);


}
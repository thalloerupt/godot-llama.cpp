#include "gdllamanpc.h"
#include "conversion.hpp"
#include "godot-llama.hpp"

#include <cstring>
#include <string>

#include <godot_cpp/core/class_db.hpp>
#include <string>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;

void GDLlamaNPC::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlamaNPC::get_model_path);
	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlamaNPC::set_model_path);
    ClassDB::add_property("GDLlamaNPC", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");

}

GDLlamaNPC::GDLlamaNPC() {

}

GDLlamaNPC::~GDLlamaNPC() {
	// Add your cleanup here.
}

void GDLlamaNPC::_process(double delta) {

}


String GDLlamaNPC::get_model_path() const {
	
    return model_path;
}

void GDLlamaNPC::set_model_path(const String p_model_path) {
	print_line(p_model_path);
    model_path = p_model_path;
}

void GDLlamaNPC::initialize() {
	// Initialize the llama model here using the model_path.
	print_line("Initializing Llama model with path: " + model_path);
	// Add your initialization code here.
	ggml_backend_load_all();
    

    // 加载模型
    llama_model_params model_params = llama_model_default_params();
    
   
    
    model_params.n_gpu_layers = 99;
    
	llama_model* model = llama_model_load_from_file(model_path.utf8().get_data(), model_params);
}
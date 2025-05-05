#include "gdllamanpc.h"
#include "gdllamaholder.h"
#include "conversion.hpp"
#include "godot-llama.hpp"

#include <format>
#include <cstring>
#include <string>
#include <godot_cpp/core/class_db.hpp>
#include <string>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <thread>

using namespace godot;

std::string prompt = R"(你是一个游戏中的NPC，请根据以下信息，以第一人称做出自然且合理的反应，并以结构化JSON格式输出结果，用于驱动游戏逻辑。
请你等待玩家的输入。
## 你是NPC，角色设定如下：
姓名：{}  
身份：{} 
可执行的动作：{}
请你模拟这个NPC的反应，并按以下格式输出：

```json
{
  "npc_name": "...",
  "emotion": "...",
  "dialogue": "...",
  "actions": [
    {
      "type": "...",
      "item": "...",
      "price": ...
    }
  ],
  "npc_state_update": {
    "trust_level": ... (可以为正负整数，代表信任变化)
  }
}


## 玩家输入（当前对话）：
{}
)";

void GDLlamaNPC::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_name"), &GDLlamaNPC::get_name);
	ClassDB::bind_method(D_METHOD("set_name", "p_name"), &GDLlamaNPC::set_name);
    ClassDB::add_property("GDLlamaNPC", PropertyInfo(Variant::STRING, "name", PROPERTY_HINT_FILE), "set_name", "get_name");

	ClassDB::bind_method(D_METHOD("get_description"), &GDLlamaNPC::get_description);
	ClassDB::bind_method(D_METHOD("set_description", "p_description"), &GDLlamaNPC::set_description);
    ClassDB::add_property("GDLlamaNPC", PropertyInfo(Variant::STRING, "description", PROPERTY_HINT_FILE), "set_description", "get_description");


	ClassDB::bind_method(D_METHOD("get_actions"), &GDLlamaNPC::get_actions);
	ClassDB::bind_method(D_METHOD("set_actions", "p_actions"), &GDLlamaNPC::set_actions);
	ClassDB::add_property("GDLlamaNPC", PropertyInfo(Variant::STRING, "actions", PROPERTY_HINT_FILE), "set_actions", "get_actions");



	ClassDB::bind_method(D_METHOD("input_action","p_input_actions"), &GDLlamaNPC::input_action);

	ADD_SIGNAL(MethodInfo("generate_text_json", PropertyInfo(Variant::STRING, "text_json")));


}

GDLlamaNPC::GDLlamaNPC() {

}

GDLlamaNPC::~GDLlamaNPC() {
	// Add your cleanup here.
}

void GDLlamaNPC::_ready() {
	if (get_parent()->is_class("GDLlamaHolder"))
	{
		llama_holder = Object::cast_to<GDLlamaHolder>(get_parent());
		print_line("MultiConversationManager is ready!");
	}
	else{
		print_error("GDLlamaNPC must be a child of GDLlamaHolder!");
		return;
	}
	_managers = std::move(llama_holder->_managers);

	id =  _managers.get() ->createConversation("Introduce yourself");
	
	

}

void GDLlamaNPC::_process(double delta) {
	

}


String GDLlamaNPC::get_name() const {
	
    return string_std_to_gd(name_npc);
}

void GDLlamaNPC::set_name(const String p_name) {
    name_npc = string_gd_to_std(p_name);
}

String GDLlamaNPC::get_description() const {
	
    return string_std_to_gd(description);
}

void GDLlamaNPC::set_description(const String p_description) {
    description = string_gd_to_std(p_description);
}


String GDLlamaNPC::get_actions() const {
	
    return string_std_to_gd(actions);
}

void GDLlamaNPC::set_actions(const String p_actions) {
    actions = string_gd_to_std(p_actions);
}


void GDLlamaNPC::input_action(const String p_input_actions) {
	std::thread([this,p_input_actions]() {

		std::string input_prompt = format(name_npc, description, actions,string_gd_to_std(p_input_actions));
		print_line_rich(string_std_to_gd(input_prompt));

		std::string tech_response = _managers.get()->generate(id, input_prompt, [this, input_prompt](const std::string& output) mutable {
			// print the token to the console
			call_deferred("emit_signal", "generate_text_json", string_std_to_gd(output));
		});
		
		

	}).detach();
	


}



#ifndef GDLLAMANPC_H
#define GDLLAMANPC_H

#include <godot_cpp/classes/node2d.hpp>
#include <string>
#include <godot_cpp/variant/string.hpp>
#include "gdllamaholder.h"

namespace godot {

class GDLlamaNPC : public Node2D {
	GDCLASS(GDLlamaNPC, Node2D)

private:
	std::string name_npc;
	std::string description;
	std::string actions;
	GDLlamaHolder *llama_holder;
	std::unique_ptr<MultiConversationManager> _managers;
	int id = -1;


protected:
	static void _bind_methods();

public:
	GDLlamaNPC();
	~GDLlamaNPC();





	String get_name() const;
	void set_name(const String p_name);

	String get_description() const;
	void set_description(const String p_description);

	String get_actions() const;
	void set_actions(const String p_actions);


	void _process(double delta) override;
	void _ready() override;
	void input_action(const String p_input_actions);
};

}

#endif
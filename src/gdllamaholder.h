#ifndef GDLLAMAHOLDER_H
#define GDLLAMAHOLDER_H

#include <godot_cpp/classes/node2d.hpp>
#include <string>
#include <godot_cpp/variant/string.hpp>

#include "godot-llama.hpp"


namespace godot {

class GDLlamaHolder : public Node2D {
	GDCLASS(GDLlamaHolder, Node2D)

private:
	std::string model_path;


protected:
	static void _bind_methods();

public:
    GDLlamaHolder();
	~GDLlamaHolder();

	String get_model_path() const;
	void set_model_path(const String p_model_path);

	std::unique_ptr<MultiConversationManager> _managers;



	void _process(double delta) override;
	void _ready() override;
	void initialize();
};

}

#endif
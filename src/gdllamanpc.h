#ifndef GDLLAMANPC_H
#define GDLLAMANPC_H

#include <godot_cpp/classes/node2d.hpp>
#include <string>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class GDLlamaNPC : public Node2D {
	GDCLASS(GDLlamaNPC, Node2D)

private:
	String model_path;
	

protected:
	static void _bind_methods();

public:
	GDLlamaNPC();
	~GDLlamaNPC();
	

	String get_model_path() const;
	void set_model_path(const String p_model_path);
	void _process(double delta) override;
	void initialize();
};

}

#endif
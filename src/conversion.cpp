#include "conversion.hpp"
#include <string>
#include <vector>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

std::vector<float> float32_array_to_vec(PackedFloat32Array array) {
    std::vector<float> vec {};
    for (float f : array) {
        vec.push_back(f);
    }
    return vec;
}

PackedFloat32Array float32_vec_to_array(std::vector<float> vec) {
    PackedFloat32Array array {};
    for (float f : vec) {
        array.push_back(f);
    }
    return array;
}

std::string string_gd_to_std(String s) {
    std::string new_s {s.utf8().get_data()};
    return new_s;
}

String string_std_to_gd(std::string s) {
    String new_s;
    new_s.parse_utf8(s.data());
    return new_s;
}

std::string format(std::string name,std::string description,std::string actions,std::string input_action){
    std::string prompt = R"(你是一个游戏中的NPC，请根据以下人物设定信息，以第一人称做出自然且合理的反应，并以结构化JSON格式输出结果，用于驱动游戏逻辑。
请你等待玩家的输入。
## 你是NPC，
你应该结合人物设定和玩家输入，从可以执行的动作中选择出合理的动作。
角色设定如下：
姓名name：)" + name +
R"(\n身份description：)"+ description +
R"(
\n可以执行的动作action：)" + actions +
R"(\n请将你的回答以逐行 JSON 格式输出，每一行是一个独立的 JSON 对象：
- 第一行为 NPC 名称
- 第二行为 NPC 当前情绪
- 第三行为 NPC 说的话
- 第四行开始为一条行为（action）
- 最后一行为 npc_state_update

示例输出：
{"npc_name": ""}
{"emotion": ""}
{"dialogue": ""}
{"action": {"type": ""}}
{"npc_state_update": {"trust_level": 3}}

不要输出额外说明，不要包含数组或嵌套结构，每一行都是一个完整JSON对象，便于流式解析，
仅输出json，不要使用markdown等格式。)" + R"(\n\n玩家输入:\n)"+input_action + R"(
)";
    return prompt


;
}




// https://stackoverflow.com/questions/28270310/how-to-easily-detect-utf8-encoding-in-the-string
bool is_utf8(const char * string)
{
    if (!string)
        return true;

    const unsigned char * bytes = (const unsigned char *)string;
    int num;

    while (*bytes != 0x00)
    {
        if ((*bytes & 0x80) == 0x00)
        {
            // U+0000 to U+007F
            num = 1;
        }
        else if ((*bytes & 0xE0) == 0xC0)
        {
            // U+0080 to U+07FF
            num = 2;
        }
        else if ((*bytes & 0xF0) == 0xE0)
        {
            // U+0800 to U+FFFF
            num = 3;
        }
        else if ((*bytes & 0xF8) == 0xF0)
        {
            // U+10000 to U+10FFFF
            num = 4;
        }
        else
            return false;

        bytes += 1;
        for (int i = 1; i < num; ++i)
        {
            if ((*bytes & 0xC0) != 0x80)
                return false;
            bytes += 1;
        }
    }

    return true;
}

} //namespace godot

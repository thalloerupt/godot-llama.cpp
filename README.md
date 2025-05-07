# Godot-llama.cpp
基于llama.cpp在Godot中实现本地运行大模型功能。性能优异，可以为NPC提供对话与动作交互功能。

## How to build
### Windows
1. 安装Cmake。
2. 运行根目录下的build.bat。
3. 将编译后的glc文件夹复制到你的Godot项目下的addons文件夹。

## Model suggestions
实测在RTX3050ti 4GB显存设备上运行Qwen2.5 3B Instruct速度约为65tok/sec，且输出内容合理。
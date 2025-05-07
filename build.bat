@echo off
REM 删除 build 文件夹（如果存在）
if exist build (
    rmdir /s /q build
    echo Deleted old build folder.
)

REM 创建新的 build 文件夹
mkdir build
echo Created new build folder.

REM 进入 build 文件夹并执行 cmake
cd build
cmake ..  -DLLAMA_CURL=OFF   -DGGML_VULKAN=ON
cmake --build . --config Release
cmake --install .

REM 暂停，以便查看输出（可选）
pause
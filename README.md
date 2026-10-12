# vedio-player
可以在cmd或powershell中播放彩色视频
# 彩色字符视频播放器
这是一个 Windows 命令行视频播放器。它使用彩色字符在终端中显示视频画面，并通过 FFplay 播放视频声音。
## 功能
- 将视频画面转换成彩色字符并实时播放
- 根据终端窗口大小调整画面
- 保持画面比例，避免拉伸
- 使用 FFplay 播放音频
## 环境要求(编译)
- 支持 C++17 的 MinGW-w64 `g++`
-  FFmpeg，且 `ffmpeg.exe` 和 `ffplay.exe`
## 环境要求(播放)
- FFmpeg，且 `ffmpeg.exe` 和 `ffplay.exe`
## 编译
在项目目录打开 PowerShell：
```powershell
g++ .\vedio.cpp -o vedio.exe -std=c++17 -Wall -Wextra
```
## 运行
先要cd到vedio.exe文件的目录
```powershell
.\vedio.exe "C:\路径\视频文件.mp4"
```
## 说明
视频画面以终端字符显示，清晰度和播放速度会受终端大小、字体及视频分辨率影响。声音由 FFplay 单独播放，因此运行时需要同时能找到 `ffplay.exe`

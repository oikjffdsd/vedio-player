# 彩色字符视频播放器
Windows 命令行视频播放器：将画面转换为彩色字符显示，并通过 FFplay 播放声音。
## 依赖
需要 `ffmpeg.exe`、`ffprobe.exe` 和 `ffplay.exe`，并确保它们所在目录已加入 Windows `PATH`。程序运行时调用这三个工具，因此目标电脑也需要安装 FFmpeg。
如果不安装，可能无法播放音频
## 编译(下载.exe文件可跳过)
静态编译：
在项目目录的 PowerShell 中运行：
```powershell
g++ .\vedio.cpp -o vedio.exe -std=c++17 -Wall -Wextra -static -static-libgcc -static-libstdc++
```
## 播放
双击 `vedio.exe` 后会打开cmd窗口，输入视频路径并回车：
```cmd
C:\Users\用户名\Videos\video.mp4
```
```cmd
"C:\Users\L1342\OneDrive\Videos\Rick Astley - Never Gonna Give You Up (Official Video) (4K Remaster)_2160p.mp4"
```

# vedio-player
可以在cmd或powershell中播放彩色视频
# 彩色字符视频播放器
这是一个 Windows 命令行视频播放器。它使用彩色字符在终端中显示视频画面，并通过 FFplay 播放视频声音。
## 功能
- 将视频画面转换成彩色字符并实时播放
- 根据终端窗口大小调整画面
- 保持画面比例，避免拉伸
- 使用 FFplay 播放音频

## 环境要求

- Windows 10 或更新版本
- 支持 C++17 的 MinGW-w64 `g++`
- FFmpeg，且 `ffmpeg.exe` 和 `ffplay.exe` 已加入 `PATH`
- 支持 ANSI/VT 控制序列的 PowerShell、Windows Terminal 或 CMD

## 编译

在项目目录打开 PowerShell：

```powershell
g++ .\vedio.cpp -o vedio.exe -std=c++17 -Wall -Wextra
```

## 运行

```powershell
.\vedio.exe "C:\路径\视频文件.mp4"
```

路径包含空格时，请保留双引号。也可以先把 `vedio.exe` 所在目录加入 `PATH`，然后直接运行：

```powershell
vedio "C:\路径\视频文件.mp4"
```

## 说明

视频画面以终端字符显示，清晰度和播放速度会受终端大小、字体及视频分辨率影响。声音由 FFplay 单独播放，因此运行时需要同时能找到 `ffplay.exe`。


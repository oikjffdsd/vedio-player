#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <windows.h>
static bool readToken(FILE *p, std::string &t)
{
    t.clear();
    int c;
    while ((c = std::fgetc(p)) != EOF)
    {
        if (c == '#')
        {
            while ((c = std::fgetc(p)) != EOF && c != '\n')
                ;
            continue;
        }
        if (c > ' ')
            break;
    }
    if (c == EOF)
        return false;
    do
        t += static_cast<char>(c), c = std::fgetc(p);
    while (c != EOF && c > ' ');
    if (c == '\r')
    {
        const int n = std::fgetc(p);
        if (n != '\n' && n != EOF)
            std::ungetc(n, p);
    }
    return true;
}
int main(int ac, char *av[])
{
    if (ac < 2)
    {
        std::cerr << "Usage: video.exe video_path\n";
        return 1;
    }
    const std::string qf = std::string("\"") + av[1] + "\"";
    const intptr_t ap = _spawnlp(_P_NOWAIT, "ffplay", "ffplay", "-nodisp", "-autoexit", "-vn", "-volume", "100", "-loglevel", "warning", qf.c_str(), nullptr);
    if (ap == -1)
        std::cerr << "Can't find ffplay, the program will only play ASCII art. Please make sure ffplay is added to PATH \n";
    else
        std::cerr << "Audio player started. \n";
    int cw = 100, ch = 28;
    HANDLE co = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND wi = GetConsoleWindow();
    if (wi)
    {
        ShowWindow(wi, SW_MAXIMIZE);
        Sleep(250);
    }
    CONSOLE_SCREEN_BUFFER_INFO ci{};
    if (GetConsoleScreenBufferInfo(co, &ci))
    {
        cw = ci.srWindow.Right - ci.srWindow.Left + 1;
        ch = ci.srWindow.Bottom - ci.srWindow.Top + 1;
    }
    cw = std::max(20, cw);
    ch = std::max(8, ch);
    DWORD cm = 0;
    if (GetConsoleMode(co, &cm))
        SetConsoleMode(co, cm | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    std::string s = "ffmpeg -nostdin -hide_banner -loglevel error -re -i \"";
    s += av[1], s += "\" -vf \"fps=24,scale=w='trunc(min(" + std::to_string(cw) + "," + std::to_string(ch) + "*2*iw/ih))':h='max(1,round(ow*ih/iw/2))',pad=" + std::to_string(cw) + ":" + std::to_string(ch) + ":(ow-iw)/2:(oh-ih)/2:color=black,format=rgb24\""
                                                                                                                                                                                                               " -f image2pipe -vcodec ppm -";
    FILE *p = _popen(s.c_str(), "rb");
    if (!p)
    {
        std::cerr << "Failed to start FFmpeg. Please make sure ffmpeg is added to PATH. \n";
        return 1;
    }
    _setmode(_fileno(p), _O_BINARY);
    const std::string cs = " .:-=+*#%@";
    bool pr = false;
    std::string t;
    while (readToken(p, t))
    {
        if (t != "P6")
        {
            std::cerr << "FFmpeg output format is invalid. \n";
            break;
        }
        std::string wt, ht, mt;
        if (!readToken(p, wt) || !readToken(p, ht) || !readToken(p, mt))
        {
            std::cerr << "Video frame data is incomplete.\n";
            break;
        }
        const int w = std::stoi(wt), h = std::stoi(ht), mv = std::stoi(mt);
        if (w <= 0 || h <= 0 || mv != 255 ||
            static_cast<long long>(w) * h > 20'000'000)
        {
            std::cerr << "Video frame dimensions or format is not supported.\n";
            break;
        }
        std::vector<unsigned char> px(static_cast<size_t>(w) * h * 3);
        if (std::fread(px.data(), 1, px.size(), p) != px.size())
        {
            std::cerr << "Failed to read video frame.\n";
            break;
        }
        std::string sc;
        sc.reserve(static_cast<size_t>(w) * h * 12);
        int pc = -1;
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                const size_t pi = (static_cast<size_t>(y) * w + x) * 3;
                const unsigned char r = px[pi], g = px[pi + 1], b = px[pi + 2], br = static_cast<unsigned char>((299 * r + 587 * g + 114 * b) / 1000);
                const size_t ix = br * (cs.size() - 1) / 255;
                const int hi = std::max({r, g, b}), lo = std::min({r, g, b});
                int cl;
                if (hi - lo < 24)
                    cl = 232 + (static_cast<int>(r) + g + b) * 23 / 765;
                else
                {
                    const int ri = (r * 5 + 127) / 255, gi = (g * 5 + 127) / 255, bi = (b * 5 + 127) / 255;
                    cl = 16 + 36 * ri + 6 * gi + bi;
                }
                if (cl != pc)
                    sc += "\x1b[38;5;" + std::to_string(cl) + "m", pc = cl;
                sc += cs[ix];
            }
            if (y + 1 < h)
                sc += "\r\x1b[1B";
        }
        if (!pr)
        {
            std::cout << "\x1b[2J";
            pr = true;
        }
        std::cout << "\x1b[H" << sc << "\x1b[0m" << std::flush;
    }
    const int rs = _pclose(p);
    if (rs != 0)
    {
        std::cerr << "FFmpeg exited with an error. \n";
        return 1;
    }
    if (ap != -1)
    {
        int ar = 0;
        _cwait(&ar, ap, _WAIT_CHILD);
        if (ar != 0)
            std::cerr << "ERROR Code:" << ar << '\n';
    }
    return 0;
}

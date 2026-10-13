#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <windows.h>
#include <fcntl.h>
#include <io.h>

static std::string qa(const std::string &s)
{
    std::string r = "\"";
    for (char c : s)
    {
        if (c == '"') r += "\\\"";
        else r += c;
    }
    return r + '"';
}

static bool getSize(const std::string &f, int &w, int &h)
{
    const std::string cm = "ffprobe -v error -select_streams v:0 -show_entries stream=width,height -of csv=p=0:s=x " + qa(f);
    FILE *p = _popen(cm.c_str(), "r");
    if (!p) return false;
    char b[128]{};
    const bool rd = std::fgets(b, sizeof(b), p) != nullptr;
    const int ec = _pclose(p);
    if (!rd || ec != 0) return false;
    return std::sscanf(b, "%dx%d", &w, &h) == 2 && w > 0 && h > 0;
}

static bool play(const std::string &f, int cw, int ch)
{
    int iw = 0, ih = 0;
    if (!getSize(f, iw, ih))
    {
        std::cerr << "Cannot read video dimensions. Make sure ffprobe is installed and the file is valid.\n";
        return false;
    }

    const int ow = std::max(1, std::min(cw, static_cast<int>(ch * 2.0 * iw / ih)));
    const int oh = std::max(1, std::min(ch, static_cast<int>(ow * static_cast<double>(ih) / iw / 2.0 + 0.5)));
    const int ox = (cw - ow) / 2, oy = (ch - oh) / 2;

    const std::string ac = "ffplay -nodisp -autoexit -loglevel error " + qa(f) + " >NUL 2>&1";
    FILE *ap = _popen(ac.c_str(), "r");
    if (!ap)
    {
        std::cerr << "Could not start ffplay. Make sure ffplay is installed and in PATH.\n";
        return false;
    }

    const std::string vc = "ffmpeg -hide_banner -loglevel error -re -i " + qa(f) +
                           " -map 0:v:0 -an -sn -dn -vf scale=" + std::to_string(ow) + ":" + std::to_string(oh) +
                           " -pix_fmt rgb24 -f rawvideo pipe:1";
    FILE *vp = _popen(vc.c_str(), "r");
    if (!vp)
    {
        _pclose(ap);
        std::cerr << "Could not start ffmpeg. Make sure ffmpeg is installed and in PATH.\n";
        return false;
    }
    _setmode(_fileno(vp), _O_BINARY);

    const std::string cs = " .:-=+*#%@";
    const size_t fs = static_cast<size_t>(ow) * oh * 3;
    std::vector<unsigned char> fr(fs);
    std::cout << "\x1b[2J\x1b[?25l" << std::flush;
    bool any = false;
    while (std::fread(fr.data(), 1, fs, vp) == fs)
    {
        any = true;
        std::string sc;
        sc.reserve(static_cast<size_t>(cw) * ch * 16);
        int pc = -1;
        for (int y = 0; y < ch; ++y)
        {
            for (int x = 0; x < cw; ++x)
            {
                int r = 0, g = 0, b = 0;
                if (x >= ox && x < ox + ow && y >= oy && y < oy + oh)
                {
                    const size_t q = (static_cast<size_t>(y - oy) * ow + (x - ox)) * 3;
                    r = fr[q]; g = fr[q + 1]; b = fr[q + 2];
                }
                const int br = (299 * r + 587 * g + 114 * b) / 1000;
                const size_t ix = static_cast<size_t>(br) * (cs.size() - 1) / 255;
                const int hi = std::max({r, g, b}), lo = std::min({r, g, b});
                const int cl = hi - lo < 24 ? 232 + (r + g + b) * 23 / 765
                    : 16 + 36 * ((r * 5 + 127) / 255) + 6 * ((g * 5 + 127) / 255) + ((b * 5 + 127) / 255);
                if (cl != pc) { sc += "\x1b[38;5;" + std::to_string(cl) + "m"; pc = cl; }
                sc += cs[ix];
            }
            if (y + 1 < ch) sc += "\r\x1b[1B";
        }
        std::cout << "\x1b[H" << sc << "\x1b[0m" << std::flush;
    }
    const int ve = _pclose(vp);
    std::cout << "\x1b[?25h\x1b[0m\n" << std::flush;
    const int ae = _pclose(ap);
    return any && ve == 0 && ae == 0;
}

int main(int ac, char *av[])
{
    std::string f;
    if (ac > 1) f = av[1];
    else
    {
        std::cout << "Enter video file path: " << std::flush;
        std::getline(std::cin, f);
    }
    if (f.size() >= 2 && f.front() == '"' && f.back() == '"')
        f = f.substr(1, f.size() - 2);
    if (f.empty())
    {
        std::cerr << "No video path was entered.\n";
        return 1;
    }

    HANDLE co = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND wi = GetConsoleWindow();
    if (wi) { ShowWindow(wi, SW_MAXIMIZE); Sleep(250); }
    CONSOLE_SCREEN_BUFFER_INFO ci{};
    int cw = 100, ch = 28;
    if (GetConsoleScreenBufferInfo(co, &ci))
    {
        cw = ci.srWindow.Right - ci.srWindow.Left + 1;
        ch = ci.srWindow.Bottom - ci.srWindow.Top + 1;
    }
    DWORD cm = 0;
    if (GetConsoleMode(co, &cm)) SetConsoleMode(co, cm | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    const bool ok = play(f, std::max(20, cw), std::max(8, ch));
    return ok ? 0 : 1;
}

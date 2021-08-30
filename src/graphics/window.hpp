#ifndef WINDOW_H
#define WINDOW_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>

namespace chip8
{
namespace graphics
{

const HBRUSH black = CreateSolidBrush(RGB(  0,   0,   0));
const HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));

class Window
{
private:
    HWND        window_handle = nullptr;
    HDC device_context_handle = nullptr;
    static LRESULT CALLBACK DefaultWindowProc(  HWND window_handle, 
                                                UINT msg, 
                                                WPARAM w_param, 
                                                LPARAM l_param  );
    WNDCLASSEX default_class = 
    {
        sizeof(WNDCLASSEX),
        CS_HREDRAW,
        DefaultWindowProc,
        0,
        0,
        GetModuleHandle(nullptr),
        LoadIcon(nullptr, IDI_APPLICATION),
        LoadCursor(nullptr, IDC_ARROW),
        white,
        nullptr,
        "default class",
        LoadIcon(nullptr, IDI_APPLICATION)
    };

public:
    Window() = delete;
    Window(const Window& w) = delete;
    Window(Window&& w) = delete;
    Window(std::string window_title, DWORD style, POINT location, int width, int height, WNDCLASSEX window_class = { 0 }, WNDPROC WindowProc = nullptr);
    virtual ~Window();

public:
    HWND GetHandle() const { return window_handle; }
    HDC GetContext() const { return device_context_handle; }
};

};
};

#endif
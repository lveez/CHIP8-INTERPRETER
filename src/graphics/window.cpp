#include "window.hpp"

namespace chip8
{
namespace graphics
{

Window::Window(std::string window_title, DWORD style, POINT location, int width, int height, WNDCLASSEX window_class, WNDPROC WindowProc)
{
    if (!window_class.cbSize)
        window_class = default_class;
    
    if (WindowProc != nullptr)
        window_class.lpfnWndProc = WindowProc;

    if (RegisterClassEx(&window_class))
    {
        window_handle = CreateWindow(window_class.lpszClassName, window_title.c_str(), style, location.x, location.y, width, height, nullptr, nullptr, GetModuleHandle(nullptr), this);
        device_context_handle = GetDC(window_handle);
    }
}

Window::~Window()
{
    if (window_handle)
        CloseHandle(window_handle);
    if (device_context_handle)
        CloseHandle(device_context_handle);
}

LRESULT CALLBACK Window::DefaultWindowProc(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param)
{
    switch(msg)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        
        default:
            return DefWindowProc(window_handle, msg, w_param, l_param);
    }
}

};
}
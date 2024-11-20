#include "gui.h"

#include <iostream>
#include <stdexcept>

#include "imgui/imgui.h"
#include "imgui/backend/imgui_impl_dx9.h"
#include "imgui/backend/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter
);

LRESULT CALLBACK WindowProcess(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter)
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
        return true;

    switch (message)
    {
    case WM_SIZE:
        {
            if (gui_t::get().m_device && wideParameter != SIZE_MINIMIZED)
            {
                gui_t::get().m_present_parameters.BackBufferWidth = LOWORD(longParameter);
                gui_t::get().m_present_parameters.BackBufferHeight = HIWORD(longParameter);
                gui_t::get().reset_device();
            }
        }
        return 0;

    case WM_DESTROY:
        {
            gui_t::get().set_is_running(false);
        }
        return 0;

        return 0;
    }

    return DefWindowProc(window, message, wideParameter, longParameter);
}


gui_t::gui_t() = default;

gui_t::~gui_t() = default;

gui_t* gui_t::create()
{
    m_singleton = std::unique_ptr<gui_t>(new gui_t);
    return m_singleton.get();
}

gui_t& gui_t::get()
{
#ifndef NDEBUG
    if (!m_singleton)
    {
        std::cerr << "Cant get gui" << std::endl;
        exit(-1);
    }
#endif

    return *m_singleton;
}


std::weak_ptr<base_window> gui_t::add_window(const std::shared_ptr<base_window>& window)
{
    m_windows.push_back(window);
    return window;
}

std::weak_ptr<base_window> gui_t::get_window_by_name(const std::string& name)
{
    const auto wi = std::ranges::find_if(m_windows, [&name](std::shared_ptr<base_window>& w)
    {
        return w->get_name() == name;
    });

    if(wi == m_windows.end())
        return {};
    return *wi;
}

void gui_t::remove_window(const std::weak_ptr<base_window>& window)
{
    if (const auto w = window.lock())
    {
        const auto wi = std::ranges::find(m_windows, w);
        if (wi != m_windows.end())
        {
            m_windows.erase(wi);
            m_windows_changed = true;
        }
    }
}

void gui_t::remove_window(const base_window* const window)
{
    if (const auto w = window)
    {
        const auto wi = std::ranges::find_if(m_windows, [w](std::shared_ptr<base_window>& sw)
        {
            return sw.get() == w;
        });
        if (wi != m_windows.end())
        {
            m_windows.erase(wi);
            m_windows_changed = true;
        }
    }
}

void gui_t::create_window(const char* win_name, const ImVec2& in_size) noexcept
{
    m_size = in_size;
    m_window_class.cbSize = sizeof(WNDCLASSEX);
    m_window_class.style = CS_CLASSDC;
    m_window_class.lpfnWndProc = WindowProcess;
    m_window_class.cbClsExtra = 0;
    m_window_class.cbWndExtra = 0;
    m_window_class.hIcon = 0;
    m_window_class.hCursor = 0;
    m_window_class.hInstance = GetModuleHandle(nullptr);
    m_window_class.hbrBackground = 0;
    m_window_class.lpszMenuName = 0;
    m_window_class.lpszClassName = "class001";
    m_window_class.hIconSm = 0;

    RegisterClassEx(&m_window_class);

    m_window = CreateWindowA(
        "class001",
        win_name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        m_size.x,
        m_size.y,
        0,
        0,
        m_window_class.hInstance,
        0
    );

    ShowWindow(m_window, SW_SHOWDEFAULT);
    UpdateWindow(m_window);
}

void gui_t::destroy_window() noexcept
{
    DestroyWindow(m_window);
    UnregisterClass(m_window_class.lpszClassName, m_window_class.hInstance);
}

bool gui_t::create_device() noexcept
{
    m_d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!m_d3d)
        return false;

    ZeroMemory(&m_present_parameters, sizeof(m_present_parameters));

    m_present_parameters.Windowed = TRUE;
    m_present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_present_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
    m_present_parameters.EnableAutoDepthStencil = TRUE;
    m_present_parameters.AutoDepthStencilFormat = D3DFMT_D16;
    m_present_parameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    const auto result = m_d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        m_window,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &m_present_parameters,
        &m_device);
    if (result != D3D_OK)
    {
        return false;
    }

    return true;
}

void gui_t::reset_device() noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    const auto result = m_device->Reset(&m_present_parameters);

    if (result == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}

void gui_t::destroy_device() noexcept
{
    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }

    if (m_d3d)
    {
        m_d3d->Release();
        m_d3d = nullptr;
    }
}

void gui_t::create_imgui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ::ImGui::GetIO();

    io.IniFilename = NULL;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(m_window);
    ImGui_ImplDX9_Init(m_device);
}

void gui_t::destroy_imgui() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void gui_t::begin_render() noexcept
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT)
        {
            m_is_running = !m_is_running;
            return;
        }
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void gui_t::end_render() noexcept
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::EndFrame();
    m_device->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f),
                                          (int)(clear_color.y*clear_color.w*255.0f),
                                          (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
    m_device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
    if (m_device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        m_device->EndScene();
    }
    HRESULT result = m_device->Present(nullptr, nullptr, nullptr, nullptr);
    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && m_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        reset_device();
}

void gui_t::render() noexcept
{
    for (const auto& w : m_windows)
    {
        w->update();

        if (m_windows_changed)
        {
            break;
        }
    }

    m_windows_changed = false;
}

void gui_t::loop() noexcept
{
    while (is_running())
    {
        begin_render();
        render();
        end_render();
    }

    destroy_imgui();
    destroy_device();
    destroy_window();
}

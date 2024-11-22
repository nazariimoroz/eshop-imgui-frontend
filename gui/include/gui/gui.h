#pragma once
#include <rfl.hpp>
#include <Windows.h>
#include <d3d9.h>
#include <memory>
#include <vector>

#include "base_window_t.h"
#include "imgui.h"


class gui_t
{
private:
    gui_t();

public:
    ~gui_t();
    gui_t(const gui_t&) = delete;
    gui_t& operator=(const gui_t&) = delete;
    gui_t(gui_t&&) = delete;
    gui_t& operator=(gui_t&&) = delete;

    static gui_t* create();

    static gui_t& get();

    [[nodiscard]] bool is_running() const { return m_is_running; }
    void set_is_running(bool is_running) { m_is_running = is_running; }

    [[nodiscard]] const ImVec2& size() const { return m_size; }

    std::weak_ptr<base_window_t> add_window(const std::shared_ptr<base_window_t>& window);
    std::weak_ptr<base_window_t> get_window_by_name(const std::string& name);
    std::shared_ptr<base_window_t> remove_window(const std::weak_ptr<base_window_t>& window);
    std::shared_ptr<base_window_t> remove_window(const base_window_t* window);


protected:
    std::vector<std::shared_ptr<base_window_t>> m_windows;
    bool m_windows_changed = false;

#pragma region Gui Drawing
public:
    void create_window(const char* win_name, const ImVec2& in_size) noexcept;
    void destroy_window() noexcept;

    bool create_device() noexcept;
    void reset_device() noexcept;
    void destroy_device() noexcept;

    void create_imgui() noexcept;
    void destroy_imgui() noexcept;

    void begin_render() noexcept;
    void end_render() noexcept;
    void render() noexcept;

    void loop() noexcept;

protected:
    bool m_is_running = true;

    HWND m_window = nullptr;
    WNDCLASSEX m_window_class = {};
    POINTS m_position = {};
    ImVec2 m_size;

protected:
    PDIRECT3D9 m_d3d = nullptr;
    LPDIRECT3DDEVICE9 m_device = nullptr;
    D3DPRESENT_PARAMETERS m_present_parameters = {};

private:
    inline static std::unique_ptr<gui_t> m_singleton = nullptr;

    friend LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);
#pragma endregion Gui Drawing
};

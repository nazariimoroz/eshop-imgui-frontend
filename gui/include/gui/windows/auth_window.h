#pragma once
#include "gui/base_window_t.h"

class auth_window_t
    : public base_window_t
{
public:
    auth_window_t();
    ~auth_window_t() override = default;

    void update() override;

    void login_tab();
    void registration_tab();

    void try_login_via_cache();
    void login();
    void registration();

    void cleanup();

protected:
    bool m_inited = false;

    char email[256] = {0};
    char password[256] = {0};
    char repeat_password[256] = {0};

protected:
    void loaded_callback(bool ok, const std::string& message);

};

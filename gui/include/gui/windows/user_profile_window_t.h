#pragma once
#include "gui/base_window_t.h"
#include "gui/gui.h"

class user_profile_window_t
    : public base_window_t
{
public:
    user_profile_window_t();
    ~user_profile_window_t() override = default;

    void update() override;

    std::shared_ptr<user_profile_window_t> logout();

protected:
    char new_email[256] = {0};
    char new_password[256] = {0};
    char new_password_repeat[256] = {0};
    char old_password[256] = {0};
};

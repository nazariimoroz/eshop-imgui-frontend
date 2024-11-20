#pragma once
#include "base_window.h"

class user_profile_window
    : public base_window
{
public:
    user_profile_window();
    ~user_profile_window() override = default;

    void update() override;

protected:
    char new_email[256] = {0};
    char new_password[256] = {0};
    char new_password_repeat[256] = {0};
    char old_password[256] = {0};
};

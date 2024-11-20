#include <iostream>

#include "gui/gui.h"
#include "gui/windows/auth_window.h"
#include "gui/windows/shop_list_window_t.h"
#include "gui/windows/user_profile_window_t.h"

int main()
{
    const auto gui = gui_t::create();
    if(!gui)
    {
        std::cerr << "Failed to create gui object" << std::endl;
        return -1;
    }

    gui->create_window("EShop", {800, 600});
    if (!gui->create_device())
    {
        std::cerr << "Cant create device" << std::endl;
        return 1;
    }
    gui->create_imgui();

    gui->add_window(std::make_shared<shop_list_window_t>());
    gui->add_window(std::make_shared<user_profile_window_t>());
    gui->add_window(std::make_shared<auth_window_t>());

    gui->loop();

    return 0;
}

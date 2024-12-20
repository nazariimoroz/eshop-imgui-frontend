#include <iostream>

#include "gui/gui.h"
#include "gui/windows/auth_window.h"
#include "gui/windows/shop_list_window_t.h"
#include "gui/windows/user_profile_window_t.h"
#include "logic/cache.h"
#include "logic/utilities/task_manager.h"

int main()
{
    const auto gui = gui_t::create();
    const auto cache = cache_t::create();
    const auto task_manager = task_manager_t::create();

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

    gui->add_window(std::make_shared<auth_window_t>());

    gui->loop();

    task_manager->wait();

    return 0;
}

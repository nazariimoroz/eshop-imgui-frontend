#include "gui/base_window_t.h"

#include <stdexcept>

base_window_t::~base_window_t()
{
    if(on_begin_destroy_callback)
        on_begin_destroy_callback();
}

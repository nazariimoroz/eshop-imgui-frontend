#include "gui/windows/shop_list_window_t.h"

#include <functional>
#include <stdexcept>

#include "gui/gui.h"
#include "imgui.h"
#include "logic/fabrics/product_fabric.h"

shop_list_window_t::shop_list_window_t()
{
    name = "ShopListWindow";
    set_show(true);
}

void shop_list_window_t::update()
{
    if(!is_shown())
        return;

    if (!m_inited)
    {
        try_get_all_products();

        m_inited = true;
    }

    const auto vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x * 2.f/3.f, vp->Size.y));
    ImGui::Begin(name.c_str(), nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBringToFrontOnFocus );

    ImGui::Text("Shop");
    ImGui::Separator();

    for (const auto& category : m_categories)
    {
        if (ImGui::TreeNode(category.name.c_str()))
        {
            for (const auto& product : category.products)
            {
                const std::string amount = (product.amount.get() == -1)
                    ? "infinity"
                    : std::to_string(product.amount.get());

                ImGui::Text("Title: %s", product.name.c_str());
                ImGui::Text("Description:\n%s", product.description.c_str());
                ImGui::Text("Amount: %s", amount.c_str() );
                ImGui::Text("Price: %i", product.price);

                ImGui::Separator();
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void shop_list_window_t::try_get_all_products()
{
    if(const auto pfabric = product_fabric_t::create())
    {
        pfabric->loaded_callback = std::bind(&shop_list_window_t::loaded_callback, this,
            std::placeholders::_1,
            std::placeholders::_2);

        const auto [ok, message] = pfabric->load_all();
        if(!ok)
        {
            std::cerr << "Failed to load: " << message << std::endl;
        }
    }
}

void shop_list_window_t::loaded_callback(
    const std::optional<product_fabric_t::ctg_models_vec_t>& models,
    const std::string& message)
{
    if(models)
    {
        m_categories = models.value();
    }
    else
    {
        std::cerr << "Error: " << message << std::endl;
    }
}

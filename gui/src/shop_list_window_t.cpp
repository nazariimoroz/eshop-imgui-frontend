#include "gui/windows/shop_list_window_t.h"

#include <functional>
#include <stdexcept>

#include "gui/gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "gui/windows/payment_window.h"
#include "logic/fabrics/product_fabric.h"

shop_list_window_t::shop_list_window_t()
{
    name = "ShopListWindow";
    set_show(true);
}

shop_list_window_t::~shop_list_window_t()
{
    auto& gui = gui_t::get();

    if(auto pw = m_payment_window.lock())
    {
        gui.remove_window(m_payment_window);
        m_payment_window.reset();
    }
}

void shop_list_window_t::update()
{
    if (!is_shown())
        return;

    if (!m_inited)
    {
        try_get_all_products();

        m_inited = true;
    }

    const auto vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x * 2.f / 3.f, vp->Size.y));
    ImGui::Begin(name.c_str(), nullptr,
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

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
                ImGui::Text("Amount: %s", amount.c_str());
                ImGui::Text("Price: %i", product.price);

                ImGui::Text("Payment methods: ");
                ImGui::PushID(&product);
                for (const auto& label : {"Google Pay", "Apple Pay"})
                {
                    ImGui::SameLine();
                    bool pushed = false;
                    if (m_payment_window.lock())
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                        pushed = true;
                    }
                    {
                        if (ImGui::Button(label))
                        {
                            open_payment_window(label, category, product);
                        }
                    }
                    if (pushed)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }
                }
                ImGui::PopID();

                ImGui::Separator();
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void shop_list_window_t::try_get_all_products()
{
    if (const auto pfabric = product_fabric_t::create())
    {
        pfabric->loaded_callback = std::bind(&shop_list_window_t::categories_loaded_callback, this,
                                             std::placeholders::_1,
                                             std::placeholders::_2);
        pfabric->context_window = gui_t::get().get_window_by_name(get_name());

        const auto [ok, message] = pfabric->load_all();
        if (!ok)
        {
            std::cerr << "Failed to load: " << message << std::endl;
        }
    }
}

void shop_list_window_t::open_payment_window(const char* payment,
                                             const category_model_t& category,
                                             const product_model_t& product)
{
    auto& gui = gui_t::get();

    if (auto w = gui.add_window(std::make_shared<payment_window_t>()).lock())
    {
        auto pw = std::dynamic_pointer_cast<payment_window_t>(w);
        m_payment_window = pw;

        pw->on_begin_destroy_callback = std::bind(
            &shop_list_window_t::paymentw_on_begin_destroy_callback,
            this);

        pw->payment = payment;
        pw->category = category;
        pw->product = product;
    }
}

void shop_list_window_t::categories_loaded_callback(
    const std::optional<std::vector<category_model_t>>& models,
    const std::string& message)
{
    if (models)
    {
        m_categories = models.value();
    }
    else
    {
        std::cerr << "Error: " << message << std::endl;
    }
}

void shop_list_window_t::paymentw_on_begin_destroy_callback()
{
    m_payment_window.reset();
}

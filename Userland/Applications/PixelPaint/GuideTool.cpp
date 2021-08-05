/*
 * Copyright (c) 2021, Tobias Christiansen <tobi@tobyase.de>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "GuideTool.h"
#include "ImageEditor.h"
#include <LibGUI/Menu.h>

namespace PixelPaint {

GuideTool::GuideTool()
{
}

GuideTool::~GuideTool()
{
}

RefPtr<Guide> GuideTool::closest_guide(const Gfx::IntPoint& point)
{
    auto guides = editor()->guides();
    Guide* closest_guide = nullptr;
    int closest_guide_distance = NumericLimits<int>::max();

    for (auto& guide : guides) {
        int relevant_position = 0;
        if (guide.orientation() == Guide::Orientation::Horizontal)
            relevant_position = point.y();
        else if (guide.orientation() == Guide::Orientation::Vertical)
            relevant_position = point.x();

        auto distance = abs(relevant_position - (int)guide.offset());

        if (distance < closest_guide_distance) {
            closest_guide = &guide;
            closest_guide_distance = distance;
        }
    }

    if (closest_guide_distance < 20)
        return closest_guide;
    return nullptr;
}

void GuideTool::on_mousedown(Layer&, GUI::MouseEvent& mouse_event, GUI::MouseEvent& image_event)
{
    if (!m_editor)
        return;

    if (mouse_event.button() != GUI::MouseButton::Left)
        return;

    RefPtr<Guide> new_guide;
    if (image_event.position().x() < 0 || image_event.position().x() > editor()->image().size().width()) {
        new_guide = Guide::construct(Guide::Orientation::Vertical, image_event.position().x());
    } else if (image_event.position().y() < 0 || image_event.position().y() > editor()->image().size().height()) {
        new_guide = Guide::construct(Guide::Orientation::Horizontal, image_event.position().y());
    }

    if (new_guide) {
        m_selected_guide = new_guide;
        m_guide_origin = 0;
        editor()->add_guide(new_guide.release_nonnull());
        return;
    }

    m_event_origin = image_event.position();

    m_selected_guide = closest_guide(image_event.position());

    if (m_selected_guide)
        m_guide_origin = m_selected_guide->offset();
}

void GuideTool::on_mouseup(Layer&, GUI::MouseEvent&, GUI::MouseEvent&)
{
    m_guide_origin = 0;
    m_event_origin = { 0, 0 };

    if (!m_selected_guide)
        return;

    if (m_selected_guide->offset() < 0
        || (m_selected_guide->orientation() == Guide::Orientation::Horizontal && m_selected_guide->offset() > editor()->image().size().height())
        || (m_selected_guide->orientation() == Guide::Orientation::Vertical && m_selected_guide->offset() > editor()->image().size().width())) {
        editor()->remove_guide(*m_selected_guide);
        editor()->layers_did_change();
    }

    m_selected_guide = nullptr;
}

void GuideTool::on_mousemove(Layer&, GUI::MouseEvent&, GUI::MouseEvent& image_event)
{
    if (!m_selected_guide)
        return;

    auto delta = image_event.position() - m_event_origin;

    auto relevant_offset = 0;
    if (m_selected_guide->orientation() == Guide::Orientation::Horizontal)
        relevant_offset = delta.y();
    else if (m_selected_guide->orientation() == Guide::Orientation::Vertical)
        relevant_offset = delta.x();

    auto new_offset = (float)relevant_offset + m_guide_origin;
    m_selected_guide->set_offset(new_offset);

    editor()->layers_did_change();
}

void GuideTool::on_context_menu(Layer&, GUI::ContextMenuEvent& event)
{
    if (!m_context_menu) {
        m_context_menu = GUI::Menu::construct();
        m_context_menu->add_action(GUI::Action::create(
            "&Delete Guide", Gfx::Bitmap::try_load_from_file("/res/icons/16x16/delete.png"), [this](auto&) {
                if (!m_context_menu_guide)
                    return;
                editor()->remove_guide(*m_context_menu_guide);
                m_selected_guide = nullptr;
                m_guide_origin = 0;
                editor()->layers_did_change();
            },
            editor()));
    }

    auto image_position = editor()->editor_position_to_image_position(event.position());
    m_context_menu_guide = closest_guide({ (int)image_position.x(), (int)image_position.y() });
    m_context_menu->popup(event.screen_position());
}

}

/*
 * Copyright (c) 2020, Matthew Olsson <mattco@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Layout/SVGGraphicsBox.h>
#include <LibWeb/SVG/SVGSVGElement.h>

namespace Web::Layout {

SVGGraphicsBox::SVGGraphicsBox(DOM::Document& document, SVG::SVGGraphicsElement& element, NonnullRefPtr<CSS::StyleProperties> properties)
    : SVGBox(document, element, properties)
{
}

void SVGGraphicsBox::before_children_paint(PaintContext& context, PaintPhase phase)
{
    SVGBox::before_children_paint(context, phase);
    if (phase != PaintPhase::Foreground)
        return;

    auto& graphics_element = verify_cast<SVG::SVGGraphicsElement>(dom_node());

    if (graphics_element.fill_color().has_value())
        context.svg_context().set_fill_color(graphics_element.fill_color().value());
    if (graphics_element.stroke_color().has_value())
        context.svg_context().set_stroke_color(graphics_element.stroke_color().value());
    if (graphics_element.stroke_width().has_value())
        context.svg_context().set_stroke_width(graphics_element.stroke_width().value());
}
float SVGGraphicsBox::viewbox_scaling()
{
    auto* svg_box = dom_node().first_ancestor_of_type<SVG::SVGSVGElement>();
    if (!svg_box || !svg_box->view_box().has_value())
        return 1;

    auto view_box = svg_box->view_box().value();

    bool has_specified_width = svg_box->has_attribute(HTML::AttributeNames::width);
    auto specified_width = svg_box->width();

    bool has_specified_height = svg_box->has_attribute(HTML::AttributeNames::height);
    auto specified_height = svg_box->height();

    auto scale_width = has_specified_width ? specified_width / view_box.width : 1;
    auto scale_height = has_specified_height ? specified_height / view_box.height : 1;

    return min(scale_width, scale_height);
}
Gfx::FloatPoint SVGGraphicsBox::viewbox_origin()
{
    auto* svg_box = dom_node().first_ancestor_of_type<SVG::SVGSVGElement>();
    if (!svg_box || !svg_box->view_box().has_value())
        return { 0, 0 };
    return { svg_box->view_box().value().min_x, svg_box->view_box().value().min_y };
}

}

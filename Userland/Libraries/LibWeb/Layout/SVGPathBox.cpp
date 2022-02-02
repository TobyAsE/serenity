/*
 * Copyright (c) 2020, Matthew Olsson <matthewcolsson@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGfx/AntiAliasingPainter.h>
#include <LibGfx/Painter.h>
#include <LibWeb/Layout/SVGPathBox.h>
#include <LibWeb/SVG/SVGPathElement.h>
#include <LibWeb/SVG/SVGSVGElement.h>

namespace Web::Layout {

SVGPathBox::SVGPathBox(DOM::Document& document, SVG::SVGPathElement& element, NonnullRefPtr<CSS::StyleProperties> properties)
    : SVGGraphicsBox(document, element, properties)
{
}

void SVGPathBox::paint(PaintContext& context, PaintPhase phase)
{
    if (!is_visible())
        return;

    SVGGraphicsBox::paint(context, phase);

    if (phase != PaintPhase::Foreground)
        return;

    auto& path_element = dom_node();
    auto path = path_element.get_path();

    auto* svg_box = path_element.first_ancestor_of_type<SVG::SVGSVGElement>();
    if (svg_box && svg_box->view_box().has_value()) {

        auto scaling = viewbox_scaling();
        auto origin = viewbox_origin();

        context.painter().clear_clip_rect();
        context.painter().add_clip_rect({ (int)absolute_position().x(), (int)absolute_position().y(), (int)svg_box->width(), (int)svg_box->height() });

        auto new_path = Gfx::Path {};
        auto transform_point = [&scaling, &origin](Gfx::FloatPoint const& point) -> Gfx::FloatPoint {
            auto new_point = point;
            new_point.translate_by({ -origin.x(), -origin.y() });
            new_point.scale_by(scaling);
            return new_point;
        };

        for (auto& segment : path.segments()) {
            switch (segment.type()) {
            case Gfx::Segment::Type::Invalid:
                break;
            case Gfx::Segment::Type::MoveTo:
                new_path.move_to(transform_point(segment.point()));
                break;
            case Gfx::Segment::Type::LineTo:
                new_path.line_to(transform_point(segment.point()));
                break;
            case Gfx::Segment::Type::QuadraticBezierCurveTo: {
                auto& quadratic_bezier_segment = static_cast<Gfx::QuadraticBezierCurveSegment const&>(segment);
                new_path.quadratic_bezier_curve_to(transform_point(quadratic_bezier_segment.through()), transform_point(quadratic_bezier_segment.point()));
                break;
            }
            case Gfx::Segment::Type::CubicBezierCurveTo: {
                auto& cubic_bezier_segment = static_cast<Gfx::CubicBezierCurveSegment const&>(segment);
                new_path.cubic_bezier_curve_to(transform_point(cubic_bezier_segment.through_0()), transform_point(cubic_bezier_segment.through_1()), transform_point(cubic_bezier_segment.point()));
                break;
            }
            case Gfx::Segment::Type::EllipticalArcTo: {
                auto& elliptical_arc_segment = static_cast<Gfx::EllipticalArcSegment const&>(segment);
                new_path.elliptical_arc_to(transform_point(elliptical_arc_segment.point()), elliptical_arc_segment.radii().scaled(scaling, scaling), elliptical_arc_segment.x_axis_rotation(), false, false);
                break;
            }
            }
        }
        path = new_path;
    }

    // We need to fill the path before applying the stroke, however the filled
    // path must be closed, whereas the stroke path may not necessary be closed.
    // Copy the path and close it for filling, but use the previous path for stroke
    auto closed_path = path;
    closed_path.close();

    // Fills are computed as though all paths are closed (https://svgwg.org/svg2-draft/painting.html#FillProperties)
    Gfx::AntiAliasingPainter painter { context.painter() };
    auto& svg_context = context.svg_context();

    auto offset = absolute_position();
    painter.translate(offset);

    painter.fill_path(
        closed_path,
        path_element.fill_color().value_or(svg_context.fill_color()),
        Gfx::Painter::WindingRule::EvenOdd);
    painter.stroke_path(
        path,
        path_element.stroke_color().value_or(svg_context.stroke_color()),
        path_element.stroke_width().value_or(svg_context.stroke_width()));

    painter.translate(-offset);
}

}

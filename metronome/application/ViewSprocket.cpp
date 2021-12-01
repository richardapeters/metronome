#include "infra/stream/StringOutputStream.hpp"
#include "metronome/application/ViewSprocket.hpp"
#include "preview/fonts/Fonts.hpp"
#include <cmath>

namespace application
{
    namespace
    {
        const auto pi = std::acos(-1);
    }

    ViewSprocket::ViewSprocket(infra::MemoryRange<const uint16_t> values, const infra::Function<void(uint16_t)>& onSelected, int adjustStart)
        : values(values)
        , onSelected(onSelected)
        , adjustStart(adjustStart)
    {}

    void ViewSprocket::Paint(hal::Canvas& canvas, infra::Region boundingRegion)
    {
        for (int tooth = 0; tooth != values.size(); ++tooth)
            if (values[tooth] != 0)
                DrawTooth(canvas, boundingRegion, tooth);
    }

    void ViewSprocket::StartTouch(infra::Point point)
    {
        startTouch = point;
        DragTo(point);
        Dirty(ViewRegion());
    }

    void ViewSprocket::DragIn(infra::Point point)
    {
        StartTouch(point);
    }

    void ViewSprocket::DragTo(infra::Point point)
    {
        auto startSelectedTooth = selectedTooth;
        selectedTooth = infra::none;

        for (int sprocket = 0; sprocket != values.size(); ++sprocket)
            if (infra::Distance(point, ToothCentre(sprocket)) <= static_cast<uint32_t>(ViewRegion().Size().deltaX / 8) && values[sprocket] != 0)
                selectedTooth = sprocket;

        if (selectedTooth != startSelectedTooth)
            Dirty(ViewRegion());
    }

    void ViewSprocket::DragOut()
    {
        StopTouch();
    }

    void ViewSprocket::StopTouch()
    {
        if (selectedTooth != infra::none)
            onSelected(values[*selectedTooth]);

        startTouch = infra::none;
        selectedTooth = infra::none;

        Dirty(ViewRegion());
    }

    void ViewSprocket::Swipe(services::Direction direction)
    {}

    services::View& ViewSprocket::GetView()
    {
        return *this;
    }

    void ViewSprocket::DrawTooth(hal::Canvas& canvas, infra::Region boundingRegion, int tooth) const
    {
        infra::StringOutputStream::WithStorage<4> value;
        value << values[tooth];

        auto point = ToothCentre(tooth);
        auto& font = infra::freeSans9pt7b;
        canvas.DrawFilledCircle(point, ViewRegion().Size().deltaX / 16, selectedTooth == tooth ? infra::Colour::red : infra::Colour::blue, boundingRegion);
        canvas.DrawString(point + infra::Vector(-font.Width(value.Storage()) / 2, font.cursorToTop / 2), value.Storage(), font, infra::Colour::white, boundingRegion);
    }

    infra::Point ViewSprocket::ToothCentre(int sprocket) const
    {
        auto offsetFromCentre = ViewRegion().Size().deltaX / 3;
        return ViewRegion().Centre()
            + infra::Vector(static_cast<int16_t>(std::cos(2 * pi * sprocket / values.size() + 2 * pi * adjustStart / 4) * offsetFromCentre)
                , static_cast<int16_t>(std::sin(2 * pi * sprocket / values.size() + 2 * pi * adjustStart / 4) * offsetFromCentre));
    }
}

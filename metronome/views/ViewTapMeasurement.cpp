#include "metronome/views/ViewTapMeasurement.hpp"
#include "preview/fonts/Fonts.hpp"
#include <numeric>

namespace application
{
    namespace
    {
        // clang-format off
        std::array<uint8_t, 32 * 28 / 8> tapBitmapData = {
            0x02, 0x10, 0x00, 0x00,
            0x22, 0x20, 0x00, 0x00,
            0x12, 0x40, 0x00, 0x00,
            0x08, 0x00, 0x00, 0x00,
            0x03, 0x80, 0x00, 0x00,
            0xe7, 0xc0, 0x00, 0x00,
            0x06, 0xe0, 0x00, 0x00,
            0x06, 0x70, 0x00, 0x00,
            0x17, 0x38, 0x3b, 0x80,
            0x23, 0x9d, 0xff, 0xc0,
            0x41, 0xcf, 0xe6, 0xe0,
            0x00, 0xe6, 0x60, 0x60,
            0x00, 0x73, 0x00, 0x70,
            0x00, 0x39, 0x00, 0x38,
            0x00, 0x18, 0x00, 0x18,
            0x00, 0x0c, 0x00, 0x1c,
            0x00, 0x3c, 0x00, 0x0c,
            0x00, 0x6c, 0x00, 0x0c,
            0x00, 0x60, 0x00, 0x0c,
            0x00, 0x60, 0x00, 0x0c,
            0x00, 0x70, 0x00, 0x0e,
            0x00, 0x3c, 0x00, 0x06,
            0x00, 0x1f, 0x00, 0x1e,
            0x00, 0x07, 0x80, 0x7c,
            0x00, 0x01, 0xc1, 0xf0,
            0x00, 0x00, 0xf3, 0xc0,
            0x00, 0x00, 0x7f, 0x00,
            0x00, 0x00, 0x1e, 0x00,
        };
        // clang-format on

        infra::Bitmap tapBitmap(tapBitmapData, infra::Vector(32, 28), infra::PixelFormat::blackandwhite);
    }

    ViewTapMeasurement::ViewTapMeasurement(BpmSelectionObserver& bpmSelectionObserver, infra::Colour inactiveColour, infra::Colour activeColour)
        : services::ViewPanel::WithView<services::ViewAligned::WithView<services::ViewIcon>>(inactiveColour, tapBitmap, infra::Colour::blue)
        , bpmSelectionObserver(bpmSelectionObserver)
        , inactiveColour(inactiveColour)
        , activeColour(activeColour)
    {}

    void ViewTapMeasurement::StartTouch(infra::Point point)
    {
        SetColour(activeColour);

        if (taps.full())
            taps.pop_front();

        taps.push_back(infra::Now());

        if (taps.size() >= 8)
            bpmSelectionObserver.BpmSelected(ComputeAverage());
    }

    void ViewTapMeasurement::DragIn(infra::Point point)
    {}

    void ViewTapMeasurement::DragTo(infra::Point point)
    {}

    void ViewTapMeasurement::DragOut()
    {
        StopTouch();
    }

    void ViewTapMeasurement::StopTouch()
    {
        SetColour(inactiveColour);
    }

    void ViewTapMeasurement::Swipe(services::Direction direction)
    {}

    services::View& ViewTapMeasurement::GetView()
    {
        return *this;
    }

    uint32_t ViewTapMeasurement::ComputeAverage() const
    {
        infra::BoundedDeque<infra::Duration>::WithMaxSize<16> intervals;
        for (auto i = taps.begin(); i != std::prev(taps.end()); ++i)
            intervals.push_back(*std::next(i) - *i);

        std::sort(intervals.begin(), intervals.end());

        while (intervals.size() > 7)
        {
            intervals.pop_front();
            intervals.pop_back();
        }

        auto average = std::accumulate(intervals.begin(), intervals.end(), infra::Duration()) / intervals.size();
        return static_cast<uint32_t>(60000 / std::chrono::duration_cast<std::chrono::milliseconds>(average).count());
    }
}

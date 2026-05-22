#pragma once

#include <algorithm>

namespace lumine::rendering {

// Smallest and largest zoom factors Lumine allows, expressed as a
// scene-to-view scale (1.0 == 100%, one image pixel per device pixel).
inline constexpr double kMinZoom = 0.02;
inline constexpr double kMaxZoom = 40.0;

// Multiplicative zoom applied per wheel notch or keyboard step.
inline constexpr double kZoomStep = 1.20;

// Clamps `scale` into the [minScale, maxScale] range.
[[nodiscard]] constexpr double clampZoom(double scale, double minScale,
                                         double maxScale) noexcept
{
    return std::clamp(scale, minScale, maxScale);
}

// Applies `steps` multiplicative zoom steps to `scale` (positive zooms in,
// negative zooms out, zero is a no-op) and clamps the result to the allowed
// range. Kept free-standing and pure so zoom boundaries are unit-testable.
[[nodiscard]] inline double steppedZoom(double scale, int steps,
                                        double minScale = kMinZoom,
                                        double maxScale = kMaxZoom) noexcept
{
    double result = scale;
    for (int i = 0; i < steps; ++i) {
        result *= kZoomStep;
    }
    for (int i = 0; i > steps; --i) {
        result /= kZoomStep;
    }
    return clampZoom(result, minScale, maxScale);
}

} // namespace lumine::rendering

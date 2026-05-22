#include <catch2/catch_test_macros.hpp>

#include "rendering/ZoomMath.hpp"

#include <cmath>

using lumine::rendering::clampZoom;
using lumine::rendering::kMaxZoom;
using lumine::rendering::kMinZoom;
using lumine::rendering::steppedZoom;

TEST_CASE("clampZoom keeps values inside the range", "[zoom]")
{
    CHECK(clampZoom(0.5, 0.1, 2.0) == 0.5);
    CHECK(clampZoom(5.0, 0.1, 2.0) == 2.0);
    CHECK(clampZoom(0.01, 0.1, 2.0) == 0.1);
}

TEST_CASE("steppedZoom zooms in and out multiplicatively", "[zoom]")
{
    CHECK(steppedZoom(1.0, 1) > 1.0);
    CHECK(steppedZoom(1.0, -1) < 1.0);
    CHECK(steppedZoom(1.0, 0) == 1.0);
    // Two steps in must exceed one step in.
    CHECK(steppedZoom(1.0, 2) > steppedZoom(1.0, 1));
}

TEST_CASE("steppedZoom clamps to the zoom limits", "[zoom][edge]")
{
    // Far more steps than the range allows must saturate, not overflow.
    CHECK(steppedZoom(1.0, 1000) == kMaxZoom);
    CHECK(steppedZoom(1.0, -1000) == kMinZoom);
}

TEST_CASE("steppedZoom is reversible for a single step", "[zoom]")
{
    const double zoomedIn = steppedZoom(1.0, 1);
    const double restored = steppedZoom(zoomedIn, -1);
    CHECK(std::abs(restored - 1.0) < 1e-9);
}

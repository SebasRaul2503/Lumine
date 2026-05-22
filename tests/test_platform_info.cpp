#include <catch2/catch_test_macros.hpp>

#include "platform/PlatformInfo.hpp"

#include <QProcessEnvironment>
#include <QString>

using lumine::platform::PlatformInfo;

TEST_CASE("PlatformInfo mirrors the session type from the environment", "[platform]")
{
    const QString expected = QProcessEnvironment::systemEnvironment().value(
        QStringLiteral("XDG_SESSION_TYPE"));

    CHECK(PlatformInfo::sessionType() == expected);
}

TEST_CASE("PlatformInfo mirrors the desktop environment", "[platform]")
{
    const QString expected = QProcessEnvironment::systemEnvironment().value(
        QStringLiteral("XDG_CURRENT_DESKTOP"));

    CHECK(PlatformInfo::desktopEnvironment() == expected);
}

TEST_CASE("PlatformInfo::isWayland is consistent with the platform name", "[platform]")
{
    // isWayland() is defined as a case-insensitive substring match against
    // the Qt platform plugin name. Verify the two never disagree, without
    // assuming which compositor the test happens to run under.
    const bool nameReportsWayland = PlatformInfo::platformName().contains(
        QStringLiteral("wayland"), Qt::CaseInsensitive);

    CHECK(PlatformInfo::isWayland() == nameReportsWayland);
}

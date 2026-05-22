#include "platform/PlatformInfo.hpp"

#include <QGuiApplication>
#include <QProcessEnvironment>

namespace lumine::platform {

QString PlatformInfo::platformName()
{
    return QGuiApplication::platformName();
}

bool PlatformInfo::isWayland()
{
    return platformName().contains(QLatin1String("wayland"), Qt::CaseInsensitive);
}

QString PlatformInfo::sessionType()
{
    return QProcessEnvironment::systemEnvironment().value(
        QStringLiteral("XDG_SESSION_TYPE"));
}

QString PlatformInfo::desktopEnvironment()
{
    return QProcessEnvironment::systemEnvironment().value(
        QStringLiteral("XDG_CURRENT_DESKTOP"));
}

} // namespace lumine::platform

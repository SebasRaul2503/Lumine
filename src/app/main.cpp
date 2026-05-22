#include "core/Logging.hpp"
#include "core/Version.hpp"
#include "platform/PlatformInfo.hpp"
#include "ui/MainWindow.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QString>
#include <QStringList>

namespace {

// Reverse-DNS application identifier. It must match the basename of
// assets/io.github.lumine.Lumine.desktop: on Wayland the compositor resolves
// the window icon and app identity from the .desktop file keyed by this name.
constexpr auto kDesktopFileName = "io.github.lumine.Lumine";

} // namespace

int main(int argc, char* argv[])
{
    // Must be set before the QApplication is constructed. PassThrough keeps
    // fractional display scaling (common on Hyprland) instead of rounding it.
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Lumine"));
    QApplication::setApplicationDisplayName(QStringLiteral("Lumine"));
    QApplication::setApplicationVersion(QString::fromLatin1(lumine::core::kVersion));
    QApplication::setOrganizationName(QStringLiteral("Lumine"));
    QGuiApplication::setDesktopFileName(QString::fromLatin1(kDesktopFileName));

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Lumine — a fast, modern image viewer for Wayland."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("image"),
                                 QStringLiteral("Image file to open on launch."));
    parser.process(app);

    qCInfo(lumine::lcApp).nospace()
        << "Lumine " << lumine::core::kVersion
        << " — platform=" << lumine::platform::PlatformInfo::platformName()
        << " session=" << lumine::platform::PlatformInfo::sessionType()
        << " desktop=" << lumine::platform::PlatformInfo::desktopEnvironment();

    if (!lumine::platform::PlatformInfo::isWayland()) {
        qCWarning(lumine::lcApp)
            << "Not running on the Wayland platform plugin. Lumine is tuned "
               "for Wayland compositors such as Hyprland.";
    }

    lumine::ui::MainWindow window;

    const QStringList positional = parser.positionalArguments();
    if (!positional.isEmpty()) {
        window.openImage(positional.first());
    }

    window.show();
    return QApplication::exec();
}

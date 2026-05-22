#include "ui/MainWindow.hpp"

#include "core/Logging.hpp"
#include "image/ImageLoader.hpp"
#include "rendering/ImageView.hpp"

#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QShortcut>
#include <QStatusBar>

#include <utility>

namespace lumine::ui {

namespace {

constexpr int kDefaultWidth = 1100;
constexpr int kDefaultHeight = 720;
constexpr int kStatusTimeoutMs = 5000;

// Builds a QFileDialog name filter from the formats Qt can decode, e.g.
// "Images (*.png *.jpg *.webp)".
QString buildImageFilter()
{
    const QStringList suffixes = image::ImageLoader::supportedSuffixes();

    QStringList patterns;
    patterns.reserve(suffixes.size());
    for (const QString& suffix : suffixes) {
        patterns.append(QStringLiteral("*.%1").arg(suffix));
    }
    return QStringLiteral("Images (%1)").arg(patterns.join(QLatin1Char(' ')));
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_imageView(new rendering::ImageView(this))
{
    buildUi();
    installShortcuts();
}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi()
{
    setCentralWidget(m_imageView);
    resize(kDefaultWidth, kDefaultHeight);
    setWindowTitle(QStringLiteral("Lumine"));

    // Compact dark styling for the chrome surrounding the canvas.
    setStyleSheet(QStringLiteral("QMainWindow { background: #121214; }"
                                 "QStatusBar { background: #19191c; color: #b8b8c0;"
                                 " border-top: 1px solid #2a2a30; }"
                                 "QStatusBar::item { border: none; }"));

    showStatus(tr("Open an image with Ctrl+O"), 0);
}

void MainWindow::installShortcuts()
{
    const auto addShortcut = [this](const QKeySequence& keys, auto&& handler) {
        auto* shortcut = new QShortcut(keys, this);
        connect(shortcut, &QShortcut::activated, this,
                std::forward<decltype(handler)>(handler));
    };

    addShortcut(QKeySequence::Open, &MainWindow::promptForImage);
    addShortcut(QKeySequence::Quit, &MainWindow::close);
    addShortcut(QKeySequence(Qt::Key_F11), &MainWindow::toggleFullscreen);
    addShortcut(QKeySequence(Qt::Key_F), &MainWindow::toggleFullscreen);
    addShortcut(QKeySequence(Qt::Key_Escape), [this]() {
        if (isFullScreen()) {
            toggleFullscreen();
        }
        else {
            close();
        }
    });
}

bool MainWindow::openImage(const QString& path)
{
    const image::LoadResult result = image::ImageLoader::load(path);
    if (!result.ok()) {
        showStatus(result.message, kStatusTimeoutMs);
        qCWarning(lcApp) << "Failed to open image:" << path;
        return false;
    }

    m_imageView->setImage(result.image);
    m_currentPath = path;

    const QString name = QFileInfo(path).fileName();
    setWindowTitle(QStringLiteral("%1 — Lumine").arg(name));
    showStatus(QStringLiteral("%1   ·   %2 × %3 px")
                   .arg(name)
                   .arg(result.image.width())
                   .arg(result.image.height()),
               kStatusTimeoutMs);
    return true;
}

void MainWindow::promptForImage()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                      m_currentPath, buildImageFilter());
    if (!path.isEmpty()) {
        openImage(path);
    }
}

void MainWindow::toggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
        statusBar()->show();
    }
    else {
        statusBar()->hide();
        showFullScreen();
    }
}

void MainWindow::showStatus(const QString& message, int timeoutMs)
{
    statusBar()->showMessage(message, timeoutMs);
}

} // namespace lumine::ui

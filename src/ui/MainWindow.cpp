#include "ui/MainWindow.hpp"

#include "core/Logging.hpp"
#include "image/AsyncImageLoader.hpp"
#include "image/ImageLoader.hpp"
#include "rendering/ImageView.hpp"
#include "ui/ThumbnailStrip.hpp"

#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QShortcut>
#include <QStatusBar>
#include <QtMath>
#include <QWidget>

#include <utility>

namespace lumine::ui {

namespace {

constexpr int kDefaultWidth = 1180;
constexpr int kDefaultHeight = 760;
constexpr int kStatusTimeoutMs = 5000;

// Decoded-image cache budget. Generous enough to hold the current image and
// several neighbours, bounded enough to keep Lumine light.
constexpr qint64 kCacheBudgetBytes = 256LL * 1024 * 1024;

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
    : QMainWindow(parent), m_imageView(new rendering::ImageView(this)),
      m_thumbnailStrip(new ThumbnailStrip(this)),
      m_loader(new image::AsyncImageLoader(this)), m_cache(kCacheBudgetBytes)
{
    buildUi();
    installShortcuts();

    connect(m_loader, &image::AsyncImageLoader::loaded, this, &MainWindow::onImageLoaded);
    connect(m_loader, &image::AsyncImageLoader::failed, this, &MainWindow::onImageFailed);
    connect(m_loader, &image::AsyncImageLoader::preloaded, this,
            &MainWindow::onImagePreloaded);
    connect(m_imageView, &rendering::ImageView::zoomChanged, this,
            &MainWindow::onZoomChanged);
    connect(m_thumbnailStrip, &ThumbnailStrip::imageSelected, this,
            &MainWindow::onThumbnailSelected);
}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi()
{
    // Central layout: thumbnail strip on the left, image canvas filling the
    // rest. The strip stays hidden until a multi-image folder is opened.
    auto* central = new QWidget(this);
    auto* layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_thumbnailStrip);
    layout->addWidget(m_imageView, 1);
    setCentralWidget(central);
    m_thumbnailStrip->hide();

    resize(kDefaultWidth, kDefaultHeight);
    setWindowTitle(QStringLiteral("Lumine"));

    setStyleSheet(QStringLiteral("QMainWindow { background: #121214; }"
                                 "QStatusBar { background: #19191c; color: #b8b8c0;"
                                 " border-top: 1px solid #2a2a30; }"
                                 "QStatusBar::item { border: none; }"));

    showStatus(tr("Open an image with Ctrl+O   ·   ←/→ navigate   ·   T strip"), 0);
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
    addShortcut(QKeySequence(Qt::Key_T), &MainWindow::toggleThumbnails);
    addShortcut(QKeySequence(Qt::Key_Escape), [this]() {
        if (isFullScreen()) {
            toggleFullscreen();
        }
        else {
            close();
        }
    });

    // Navigation.
    addShortcut(QKeySequence(Qt::Key_Right), &MainWindow::showNext);
    addShortcut(QKeySequence(Qt::Key_Space), &MainWindow::showNext);
    addShortcut(QKeySequence(Qt::Key_Left), &MainWindow::showPrevious);

    // Zoom.
    addShortcut(QKeySequence::ZoomIn, [this]() { m_imageView->zoomIn(); });
    addShortcut(QKeySequence(Qt::Key_Plus), [this]() { m_imageView->zoomIn(); });
    addShortcut(QKeySequence(Qt::Key_Equal), [this]() { m_imageView->zoomIn(); });
    addShortcut(QKeySequence::ZoomOut, [this]() { m_imageView->zoomOut(); });
    addShortcut(QKeySequence(Qt::Key_Minus), [this]() { m_imageView->zoomOut(); });
    addShortcut(QKeySequence(Qt::Key_0), [this]() { m_imageView->zoomToFit(); });
    addShortcut(QKeySequence(Qt::Key_1), [this]() { m_imageView->zoomToActualSize(); });
}

void MainWindow::openImage(const QString& path)
{
    const QFileInfo info(path);
    if (!info.exists() || !info.isFile()) {
        showStatus(tr("File not found: %1").arg(path), kStatusTimeoutMs);
        qCWarning(lcApp) << "openImage: missing file" << path;
        return;
    }

    m_imageList.openFromFile(info.absoluteFilePath());
    m_cache.clear(); // a new directory — drop the old neighbourhood
    m_thumbnailStrip->setPaths(m_imageList.paths());
    m_thumbnailStrip->setVisible(m_imageList.count() > 1);
    loadCurrent();
}

void MainWindow::loadCurrent()
{
    const QString path = m_imageList.current();
    if (path.isEmpty()) {
        return;
    }

    m_thumbnailStrip->setCurrentImage(m_imageList.currentIndex());

    if (m_cache.contains(path)) {
        displayImage(path, m_cache.get(path));
    }
    else {
        showStatus(tr("Loading %1…").arg(QFileInfo(path).fileName()), 0);
        m_loader->request(path);
    }
    preloadNeighbours();
}

void MainWindow::displayImage(const QString& path, const QImage& image)
{
    m_displayedPath = path;
    m_imageView->setImage(image);
    updateTitle();
    showStatus(QStringLiteral("%1   ·   %2 × %3   ·   %4 / %5")
                   .arg(QFileInfo(path).fileName())
                   .arg(image.width())
                   .arg(image.height())
                   .arg(m_imageList.currentIndex() + 1)
                   .arg(m_imageList.count()),
               kStatusTimeoutMs);
}

void MainWindow::preloadNeighbours()
{
    QStringList unique;
    for (const int offset : {1, -1}) {
        const QString neighbour = m_imageList.peek(offset);
        if (!neighbour.isEmpty() && neighbour != m_imageList.current() &&
            !m_cache.contains(neighbour) && !unique.contains(neighbour)) {
            unique.append(neighbour);
        }
    }
    for (const QString& neighbour : unique) {
        m_loader->requestPreload(neighbour);
    }
}

void MainWindow::showNext()
{
    if (m_imageList.count() < 2) {
        return;
    }
    m_imageList.next();
    loadCurrent();
}

void MainWindow::showPrevious()
{
    if (m_imageList.count() < 2) {
        return;
    }
    m_imageList.previous();
    loadCurrent();
}

void MainWindow::onImageLoaded(const QString& path, const QImage& image)
{
    m_cache.insert(path, image);
    // The generation token means this is the latest foreground result; the
    // guard simply avoids a redundant repaint after fast navigation. The
    // neighbours were already queued by loadCurrent(), so no preload here.
    if (path == m_imageList.current()) {
        displayImage(path, image);
    }
}

void MainWindow::onImageFailed(const QString& path, const QString& message)
{
    showStatus(message, kStatusTimeoutMs);
    qCWarning(lcApp) << "Load failed:" << path << message;
}

void MainWindow::onImagePreloaded(const QString& path, const QImage& image)
{
    m_cache.insert(path, image);
}

void MainWindow::onThumbnailSelected(int index)
{
    if (m_imageList.setCurrentIndex(index)) {
        loadCurrent();
    }
}

void MainWindow::onZoomChanged(double /*factor*/)
{
    updateTitle();
}

void MainWindow::promptForImage()
{
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Open Image"), m_displayedPath, buildImageFilter());
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

void MainWindow::toggleThumbnails()
{
    m_thumbnailStrip->setVisible(!m_thumbnailStrip->isVisible());
}

void MainWindow::showStatus(const QString& message, int timeoutMs)
{
    statusBar()->showMessage(message, timeoutMs);
}

void MainWindow::updateTitle()
{
    if (m_displayedPath.isEmpty()) {
        setWindowTitle(QStringLiteral("Lumine"));
        return;
    }
    const QString name = QFileInfo(m_displayedPath).fileName();
    const int percent = qRound(m_imageView->zoomFactor() * 100.0);
    setWindowTitle(QStringLiteral("%1  ·  %2%  —  Lumine").arg(name).arg(percent));
}

} // namespace lumine::ui

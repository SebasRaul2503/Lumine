#pragma once

#include "core/ImageCache.hpp"
#include "core/RecentFiles.hpp"
#include "image/ImageList.hpp"

#include <QImage>
#include <QMainWindow>
#include <QString>

class QTimer;

namespace lumine::image {
class AsyncImageLoader;
}
namespace lumine::rendering {
class ImageView;
}
namespace lumine::ui {
class InfoPanel;
class ThumbnailStrip;
} // namespace lumine::ui

namespace lumine::ui {

// The application's top-level window. It hosts the image canvas, the
// thumbnail strip and the info panel; owns the keyboard shortcuts, the
// navigable image list, the slideshow timer and the recent-files list; and
// orchestrates the cache and asynchronous loading. Each concern is delegated
// to a dedicated component — MainWindow only wires them together.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

public slots:
    // Opens `path`: builds the navigable list from its directory and starts
    // an asynchronous load of the image itself.
    void openImage(const QString& path);

private slots:
    void promptForImage();
    void showRecentMenu();
    void toggleFullscreen();
    void toggleThumbnails();
    void toggleInfoPanel();
    void toggleSlideshow();
    void showNext();
    void showPrevious();
    void onImageLoaded(const QString& path, const QImage& image);
    void onImageFailed(const QString& path, const QString& message);
    void onImagePreloaded(const QString& path, const QImage& image);
    void onThumbnailSelected(int index);
    void onZoomChanged(double factor);

private:
    void buildUi();
    void installShortcuts();
    void loadCurrent();
    void displayImage(const QString& path, const QImage& image);
    void preloadNeighbours();
    void rememberRecent(const QString& path);
    void showStatus(const QString& message, int timeoutMs);
    void updateTitle();

    // Parented to this window via Qt's object tree.
    rendering::ImageView* m_imageView = nullptr;
    ThumbnailStrip* m_thumbnailStrip = nullptr;
    InfoPanel* m_infoPanel = nullptr;
    image::AsyncImageLoader* m_loader = nullptr;
    QTimer* m_slideshowTimer = nullptr;

    image::ImageList m_imageList;
    core::ImageCache m_cache;
    core::RecentFiles m_recentFiles;
    QString m_displayedPath;
};

} // namespace lumine::ui

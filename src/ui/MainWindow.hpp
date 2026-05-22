#pragma once

#include "core/ImageCache.hpp"
#include "image/ImageList.hpp"

#include <QImage>
#include <QMainWindow>
#include <QString>

namespace lumine::image {
class AsyncImageLoader;
}
namespace lumine::rendering {
class ImageView;
}
namespace lumine::ui {
class ThumbnailStrip;
}

namespace lumine::ui {

// The application's top-level window. It hosts the image canvas and the
// thumbnail strip, owns the keyboard shortcuts and the navigable image list,
// and orchestrates the cache and asynchronous loading — but delegates
// rendering, decoding and thumbnailing to dedicated components.
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
    void toggleFullscreen();
    void toggleThumbnails();
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
    void showStatus(const QString& message, int timeoutMs);
    void updateTitle();

    // Parented to this window via Qt's object tree.
    rendering::ImageView* m_imageView = nullptr;
    ThumbnailStrip* m_thumbnailStrip = nullptr;
    image::AsyncImageLoader* m_loader = nullptr;

    image::ImageList m_imageList;
    core::ImageCache m_cache;
    QString m_displayedPath;
};

} // namespace lumine::ui

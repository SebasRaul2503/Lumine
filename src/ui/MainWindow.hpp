#pragma once

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

// The application's top-level window. It hosts the image canvas, owns the
// keyboard shortcuts and the navigable image list, and drives asynchronous
// loading — but delegates rendering to ImageView and decoding to the loader.
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
    void showNext();
    void showPrevious();
    void onImageLoaded(const QString& path, const QImage& image);
    void onImageFailed(const QString& path, const QString& message);
    void onZoomChanged(double factor);

private:
    void buildUi();
    void installShortcuts();
    void loadCurrent();
    void showStatus(const QString& message, int timeoutMs);
    void updateTitle();

    // Parented to this window via Qt's object tree.
    rendering::ImageView* m_imageView = nullptr;
    image::AsyncImageLoader* m_loader = nullptr;
    image::ImageList m_imageList;
    QString m_displayedPath;
};

} // namespace lumine::ui

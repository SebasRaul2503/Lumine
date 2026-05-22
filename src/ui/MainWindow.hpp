#pragma once

#include <QMainWindow>
#include <QString>

namespace lumine::rendering {
class ImageView;
}

namespace lumine::ui {

// The application's top-level window. It hosts the image canvas, owns the
// keyboard shortcuts and surfaces load status, but delegates all rendering to
// ImageView and all decoding to ImageLoader.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

public slots:
    // Loads and displays the image at `path`. On failure the window is left
    // unchanged and a status message is shown. Returns true on success.
    bool openImage(const QString& path);

private slots:
    void promptForImage();
    void toggleFullscreen();

private:
    void buildUi();
    void installShortcuts();
    void showStatus(const QString& message, int timeoutMs);

    // Parented to this window via Qt's object tree.
    rendering::ImageView* m_imageView = nullptr;
    QString m_currentPath;
};

} // namespace lumine::ui

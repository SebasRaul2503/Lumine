#pragma once

#include <QGraphicsView>
#include <QSize>

class QGraphicsPixmapItem;
class QGraphicsScene;
class QImage;
class QVariantAnimation;

namespace lumine::rendering {

// A GPU-friendly canvas that displays one image with interactive zoom, pan
// and a subtle fade-in when the image changes. It renders whatever QImage it
// is handed and knows nothing about files, navigation or application state.
class ImageView : public QGraphicsView {
    Q_OBJECT

public:
    // How the current scale is derived.
    enum class ZoomMode {
        Fit,    // scaled to fit the viewport; tracks resizes
        Actual, // 1:1 with the image's pixels
        Free,   // an explicit factor chosen with the wheel or keyboard
    };

    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() override;

    // Displays `image`; a null image clears the canvas. The zoom resets to
    // Fit and the new image fades in.
    void setImage(const QImage& image);
    void clear();

    [[nodiscard]] bool hasImage() const noexcept;

    // Effective scale as a scene-to-view factor (1.0 == 100%).
    [[nodiscard]] double zoomFactor() const noexcept;
    [[nodiscard]] ZoomMode zoomMode() const noexcept;

public slots:
    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void zoomToActualSize();

signals:
    // Emitted whenever the effective scale changes.
    void zoomChanged(double factor);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    // Scale that makes the image exactly fit the current viewport.
    [[nodiscard]] double fitScale() const;
    void applyScale(double scale);
    void setZoom(double scale, ZoomMode mode);
    void refreshDragMode();
    void startFadeIn();

    // Owned by Qt's parent-child tree: m_scene is parented to this view,
    // m_pixmapItem is owned by m_scene, m_fadeAnimation is parented here.
    QGraphicsScene* m_scene = nullptr;
    QGraphicsPixmapItem* m_pixmapItem = nullptr;
    QVariantAnimation* m_fadeAnimation = nullptr;
    QSize m_imageSize;
    ZoomMode m_zoomMode = ZoomMode::Fit;
    double m_scale = 1.0;
};

} // namespace lumine::rendering

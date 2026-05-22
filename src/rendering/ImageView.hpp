#pragma once

#include <QGraphicsView>

class QGraphicsPixmapItem;
class QGraphicsScene;
class QImage;

namespace lumine::rendering {

// A GPU-friendly canvas that displays a single image.
//
// The image is always scaled to fit the viewport while preserving its aspect
// ratio, and is re-fitted whenever the widget is resized. Interactive zoom
// and pan are introduced in Phase 2 — see docs/roadmap.md.
//
// ImageView deliberately knows nothing about files, navigation or the rest of
// the UI: it renders whatever QImage it is handed. This keeps rendering
// concerns separate from application logic.
class ImageView : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() override;

    // Displays `image`. Passing a null image clears the canvas.
    void setImage(const QImage& image);

    // Removes any displayed image.
    void clear();

    [[nodiscard]] bool hasImage() const noexcept;

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void fitImageToViewport();

    // Owned by Qt's parent-child tree: m_scene is parented to this view and
    // m_pixmapItem is owned by m_scene. They are observed, not owned, here.
    QGraphicsScene* m_scene = nullptr;
    QGraphicsPixmapItem* m_pixmapItem = nullptr;
};

} // namespace lumine::rendering

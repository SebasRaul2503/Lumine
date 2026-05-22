#include "rendering/ImageView.hpp"

#include <QColor>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>
#include <QShowEvent>

namespace lumine::rendering {

namespace {

// Neutral near-black backdrop for the dark-first interface.
const QColor kCanvasBackground{0x12, 0x12, 0x14};

} // namespace

ImageView::ImageView(QWidget* parent)
    : QGraphicsView(parent), m_scene(new QGraphicsScene(this))
{
    setScene(m_scene);

    // The image is the interface: no frame, no scrollbars.
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignCenter);
    setDragMode(QGraphicsView::NoDrag);

    // Smooth, GPU-friendly rendering.
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::Antialiasing, true);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);

    setBackgroundBrush(kCanvasBackground);
}

ImageView::~ImageView() = default;

void ImageView::setImage(const QImage& image)
{
    if (image.isNull()) {
        clear();
        return;
    }

    const QPixmap pixmap = QPixmap::fromImage(image);

    if (m_pixmapItem == nullptr) {
        m_pixmapItem = m_scene->addPixmap(pixmap);
        m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    }
    else {
        m_pixmapItem->setPixmap(pixmap);
    }

    m_scene->setSceneRect(m_pixmapItem->boundingRect());
    fitImageToViewport();
}

void ImageView::clear()
{
    m_scene->clear(); // deletes every item, including m_pixmapItem
    m_pixmapItem = nullptr;
    m_scene->setSceneRect(QRectF{});
}

bool ImageView::hasImage() const noexcept
{
    return m_pixmapItem != nullptr;
}

void ImageView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    fitImageToViewport();
}

void ImageView::showEvent(QShowEvent* event)
{
    QGraphicsView::showEvent(event);
    fitImageToViewport();
}

void ImageView::fitImageToViewport()
{
    if (m_pixmapItem == nullptr) {
        return;
    }
    fitInView(m_pixmapItem, Qt::KeepAspectRatio);
}

} // namespace lumine::rendering

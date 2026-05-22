#include "rendering/ImageView.hpp"

#include "rendering/ZoomMath.hpp"

#include <QColor>
#include <QEasingCurve>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTransform>
#include <QVariantAnimation>
#include <QWheelEvent>

#include <algorithm>

namespace lumine::rendering {

namespace {
// Neutral near-black backdrop for the dark-first interface.
const QColor kCanvasBackground{0x12, 0x12, 0x14};

// Slack above the fit scale before panning is offered, to absorb rounding.
constexpr double kPanThreshold = 0.001;

// Duration of the fade-in shown when the image changes.
constexpr int kFadeDurationMs = 130;
} // namespace

ImageView::ImageView(QWidget* parent)
    : QGraphicsView(parent), m_scene(new QGraphicsScene(this)),
      m_fadeAnimation(new QVariantAnimation(this))
{
    setScene(m_scene);

    // The image is the interface: no frame, no scrollbars.
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignCenter);
    setDragMode(QGraphicsView::NoDrag);

    // Smooth, GPU-friendly rendering. Wheel zoom pivots on the cursor.
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::Antialiasing, true);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);

    setBackgroundBrush(kCanvasBackground);

    m_fadeAnimation->setDuration(kFadeDurationMs);
    m_fadeAnimation->setStartValue(0.0);
    m_fadeAnimation->setEndValue(1.0);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_fadeAnimation, &QVariantAnimation::valueChanged, this,
            [this](const QVariant& value) {
                if (m_pixmapItem != nullptr) {
                    m_pixmapItem->setOpacity(value.toDouble());
                }
            });
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

    m_imageSize = image.size();
    m_scene->setSceneRect(m_pixmapItem->boundingRect());
    zoomToFit();
    startFadeIn();
}

void ImageView::clear()
{
    m_fadeAnimation->stop();
    m_scene->clear(); // deletes every item, including m_pixmapItem
    m_pixmapItem = nullptr;
    m_imageSize = QSize();
    m_scene->setSceneRect(QRectF{});
}

bool ImageView::hasImage() const noexcept
{
    return m_pixmapItem != nullptr;
}

double ImageView::zoomFactor() const noexcept
{
    return m_scale;
}

ImageView::ZoomMode ImageView::zoomMode() const noexcept
{
    return m_zoomMode;
}

void ImageView::zoomIn()
{
    setZoom(steppedZoom(m_scale, 1), ZoomMode::Free);
}

void ImageView::zoomOut()
{
    setZoom(steppedZoom(m_scale, -1), ZoomMode::Free);
}

void ImageView::zoomToFit()
{
    m_zoomMode = ZoomMode::Fit;
    applyScale(fitScale());
}

void ImageView::zoomToActualSize()
{
    setZoom(1.0, ZoomMode::Actual);
}

void ImageView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    // Only Fit tracks the viewport; explicit zoom levels are preserved.
    if (m_zoomMode == ZoomMode::Fit) {
        applyScale(fitScale());
    }
}

void ImageView::showEvent(QShowEvent* event)
{
    QGraphicsView::showEvent(event);
    if (m_zoomMode == ZoomMode::Fit) {
        applyScale(fitScale());
    }
}

void ImageView::wheelEvent(QWheelEvent* event)
{
    if (m_pixmapItem == nullptr || event->angleDelta().y() == 0) {
        return;
    }
    const int steps = event->angleDelta().y() > 0 ? 1 : -1;
    setZoom(steppedZoom(m_scale, steps), ZoomMode::Free);
    event->accept();
}

void ImageView::mouseDoubleClickEvent(QMouseEvent* event)
{
    // Double click toggles between fitting the window and 1:1 pixels.
    if (m_zoomMode == ZoomMode::Fit) {
        zoomToActualSize();
    }
    else {
        zoomToFit();
    }
    event->accept();
}

double ImageView::fitScale() const
{
    if (m_imageSize.isEmpty()) {
        return 1.0;
    }
    const QSize available = viewport()->size();
    const double sx =
        static_cast<double>(available.width()) / static_cast<double>(m_imageSize.width());
    const double sy = static_cast<double>(available.height()) /
                      static_cast<double>(m_imageSize.height());
    return clampZoom(std::min(sx, sy), kMinZoom, kMaxZoom);
}

void ImageView::applyScale(double scale)
{
    m_scale = scale;
    setTransform(QTransform::fromScale(scale, scale));
    refreshDragMode();
    emit zoomChanged(m_scale);
}

void ImageView::setZoom(double scale, ZoomMode mode)
{
    if (m_pixmapItem == nullptr) {
        return;
    }
    m_zoomMode = mode;
    applyScale(clampZoom(scale, kMinZoom, kMaxZoom));
}

void ImageView::refreshDragMode()
{
    // Hand-drag panning is offered only when the image overflows the view.
    const bool pannable = m_pixmapItem != nullptr && m_scale > fitScale() + kPanThreshold;
    setDragMode(pannable ? QGraphicsView::ScrollHandDrag : QGraphicsView::NoDrag);
}

void ImageView::startFadeIn()
{
    if (m_pixmapItem == nullptr) {
        return;
    }
    m_fadeAnimation->stop();
    m_pixmapItem->setOpacity(0.0);
    m_fadeAnimation->start();
}

} // namespace lumine::rendering

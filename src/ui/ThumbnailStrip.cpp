#include "ui/ThumbnailStrip.hpp"

#include "image/ThumbnailLoader.hpp"

#include <QFileInfo>
#include <QIcon>
#include <QImage>
#include <QListWidgetItem>
#include <QPixmap>
#include <QSignalBlocker>
#include <QSize>

namespace lumine::ui {

namespace {
constexpr int kThumbnailEdge = 116;
constexpr int kStripWidth = 152;
} // namespace

ThumbnailStrip::ThumbnailStrip(QWidget* parent)
    : QListWidget(parent), m_loader(new image::ThumbnailLoader(this))
{
    setViewMode(QListView::IconMode);
    setMovement(QListView::Static);
    setResizeMode(QListView::Adjust);
    setFlow(QListView::TopToBottom);
    setWrapping(false);
    setUniformItemSizes(true);
    setIconSize(QSize(kThumbnailEdge, kThumbnailEdge));
    setFixedWidth(kStripWidth);
    setSpacing(4);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTextElideMode(Qt::ElideMiddle);
    // The strip never takes keyboard focus, so the window's arrow-key
    // navigation always reaches the image rather than the list.
    setFocusPolicy(Qt::NoFocus);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setStyleSheet(QStringLiteral(
        "QListWidget { background: #161618; border-right: 1px solid #2a2a30; }"
        "QListWidget::item { color: #9a9aa2; padding: 3px; border-radius: 6px; }"
        "QListWidget::item:selected { background: #2d2d6b; color: #ffffff; }"));

    connect(m_loader, &image::ThumbnailLoader::ready, this,
            &ThumbnailStrip::onThumbnailReady);
    connect(this, &QListWidget::itemClicked, this,
            [this](QListWidgetItem* item) { emit imageSelected(row(item)); });
}

ThumbnailStrip::~ThumbnailStrip() = default;

void ThumbnailStrip::setPaths(const QStringList& paths)
{
    m_loader->clear();
    clear();

    for (const QString& path : paths) {
        auto* item = new QListWidgetItem(QFileInfo(path).fileName(), this);
        item->setToolTip(path);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
        item->setSizeHint(QSize(kThumbnailEdge + 16, kThumbnailEdge + 26));
    }

    for (int i = 0; i < paths.size(); ++i) {
        m_loader->request(i, paths.at(i), QSize(kThumbnailEdge, kThumbnailEdge));
    }
}

void ThumbnailStrip::setCurrentImage(int index)
{
    if (index < 0 || index >= count()) {
        return;
    }
    // Block signals so this programmatic selection is not mistaken for a
    // click and bounced back to the host.
    const QSignalBlocker blocker(this);
    setCurrentRow(index);
    scrollToItem(item(index), QAbstractItemView::PositionAtCenter);
}

void ThumbnailStrip::onThumbnailReady(int id, const QImage& thumbnail)
{
    if (id < 0 || id >= count()) {
        return;
    }
    item(id)->setIcon(QIcon(QPixmap::fromImage(thumbnail)));
}

} // namespace lumine::ui

#include "image/ThumbnailLoader.hpp"

#include "image/ImageLoader.hpp"

#include <QRunnable>
#include <QThreadPool>

namespace lumine::image {

namespace {
constexpr int kThumbnailThreads = 2;
} // namespace

ThumbnailLoader::ThumbnailLoader(QObject* parent)
    : QObject(parent), m_pool(new QThreadPool(this))
{
    m_pool->setMaxThreadCount(kThumbnailThreads);
}

ThumbnailLoader::~ThumbnailLoader()
{
    m_pool->clear();
    m_pool->waitForDone();
}

void ThumbnailLoader::request(int id, const QString& path, const QSize& maxSize)
{
    m_pool->start(QRunnable::create([this, id, path, maxSize]() {
        const QImage thumbnail = ImageLoader::loadThumbnail(path, maxSize);
        if (thumbnail.isNull()) {
            return; // a thumbnail that cannot decode is simply skipped
        }
        QMetaObject::invokeMethod(
            this, [this, id, thumbnail]() { emit ready(id, thumbnail); },
            Qt::QueuedConnection);
    }));
}

void ThumbnailLoader::clear()
{
    m_pool->clear();
}

} // namespace lumine::image

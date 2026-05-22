#include "image/AsyncImageLoader.hpp"

#include "core/Logging.hpp"
#include "image/ImageLoader.hpp"

#include <QRunnable>
#include <QThreadPool>

namespace lumine::image {

namespace {
// Decoding is an I/O wait plus a CPU burst. A small pool bounds memory while
// still letting a foreground decode overlap a speculative preload.
constexpr int kMaxDecodeThreads = 2;

// QThreadPool runs higher-priority runnables first; the visible image must
// not wait behind speculative preloads.
constexpr int kForegroundPriority = 1;
constexpr int kPreloadPriority = 0;
} // namespace

AsyncImageLoader::AsyncImageLoader(QObject* parent)
    : QObject(parent), m_pool(new QThreadPool(this))
{
    m_pool->setMaxThreadCount(kMaxDecodeThreads);
}

AsyncImageLoader::~AsyncImageLoader()
{
    // Drop queued work and wait for running decodes so no worker can touch
    // this object after destruction begins.
    m_pool->clear();
    m_pool->waitForDone();
}

void AsyncImageLoader::request(const QString& path)
{
    const quint64 generation = ++m_generation;

    m_pool->start(QRunnable::create([this, path, generation]() {
                      const LoadResult result = ImageLoader::load(path);
                      QMetaObject::invokeMethod(
                          this,
                          [this, path, generation, result]() {
                              deliver(path, generation, result.ok(), result.image,
                                      result.message);
                          },
                          Qt::QueuedConnection);
                  }),
                  kForegroundPriority);
}

void AsyncImageLoader::requestPreload(const QString& path)
{
    m_pool->start(
        QRunnable::create([this, path]() {
            const LoadResult result = ImageLoader::load(path);
            if (!result.ok()) {
                return; // preload failures are silent
            }
            QMetaObject::invokeMethod(
                this,
                [this, path, image = result.image]() { deliverPreload(path, image); },
                Qt::QueuedConnection);
        }),
        kPreloadPriority);
}

void AsyncImageLoader::cancel()
{
    ++m_generation; // any pending foreground result is now stale
    m_pool->clear();
}

void AsyncImageLoader::deliver(const QString& path, quint64 generation, bool ok,
                               const QImage& image, const QString& message)
{
    if (generation != m_generation) {
        qCDebug(lcImage) << "Dropping superseded result for" << path;
        return;
    }

    if (ok) {
        emit loaded(path, image);
    }
    else {
        emit failed(path, message);
    }
}

void AsyncImageLoader::deliverPreload(const QString& path, const QImage& image)
{
    qCDebug(lcImage) << "Preloaded" << path;
    emit preloaded(path, image);
}

} // namespace lumine::image

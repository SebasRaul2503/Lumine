#pragma once

#include <QImage>
#include <QObject>
#include <QString>

class QThreadPool;

namespace lumine::image {

// Decodes images on a background thread pool and delivers results to the
// thread that owns this object via queued signals, so the UI thread never
// blocks on I/O or decoding.
//
// Foreground requests carry a monotonic generation token: when a newer
// request arrives the token advances, and superseded results are dropped, so
// rapid navigation never flashes a stale image. Preload requests are
// speculative, run at a lower priority, and always deliver into the cache
// regardless of generation.
class AsyncImageLoader : public QObject {
    Q_OBJECT

public:
    explicit AsyncImageLoader(QObject* parent = nullptr);
    ~AsyncImageLoader() override;

    // Requests an asynchronous decode of `path`, superseding any pending
    // foreground request. Emits loaded() or failed().
    void request(const QString& path);

    // Speculatively decodes a neighbour image. Emits preloaded() on success
    // and stays silent on failure. Never supersedes a foreground request.
    void requestPreload(const QString& path);

    // Drops every in-flight foreground request without emitting a result.
    void cancel();

signals:
    void loaded(const QString& path, const QImage& image);
    void failed(const QString& path, const QString& message);
    void preloaded(const QString& path, const QImage& image);

private:
    void deliver(const QString& path, quint64 generation, bool ok, const QImage& image,
                 const QString& message);
    void deliverPreload(const QString& path, const QImage& image);

    QThreadPool* m_pool = nullptr; // owned via Qt's parent-child tree
    quint64 m_generation = 0;
};

} // namespace lumine::image

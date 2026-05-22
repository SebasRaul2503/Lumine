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
// Every request carries a monotonic generation token. When a newer request
// arrives the token advances, and results from superseded requests are
// silently dropped — rapid navigation never flashes a stale image.
class AsyncImageLoader : public QObject {
    Q_OBJECT

public:
    explicit AsyncImageLoader(QObject* parent = nullptr);
    ~AsyncImageLoader() override;

    // Requests an asynchronous decode of `path`, superseding any pending
    // request that has not yet been delivered.
    void request(const QString& path);

    // Drops every in-flight request without emitting a result.
    void cancel();

signals:
    void loaded(const QString& path, const QImage& image);
    void failed(const QString& path, const QString& message);

private:
    void deliver(const QString& path, quint64 generation, bool ok, const QImage& image,
                 const QString& message);

    QThreadPool* m_pool = nullptr; // owned via Qt's parent-child tree
    quint64 m_generation = 0;
};

} // namespace lumine::image

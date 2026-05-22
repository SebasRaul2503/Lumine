#pragma once

#include <QImage>
#include <QObject>
#include <QSize>
#include <QString>

class QThreadPool;

namespace lumine::image {

// Decodes down-scaled thumbnails on a background thread pool and delivers
// them to the owning thread via a queued signal.
//
// Each request carries an opaque integer `id` chosen by the caller (Lumine
// passes the strip row), echoed back with the result so the caller can route
// it without tracking paths.
class ThumbnailLoader : public QObject {
    Q_OBJECT

public:
    explicit ThumbnailLoader(QObject* parent = nullptr);
    ~ThumbnailLoader() override;

    // Queues a thumbnail decode of `path` bounded by `maxSize`.
    void request(int id, const QString& path, const QSize& maxSize);

    // Drops queued requests that have not started yet.
    void clear();

signals:
    void ready(int id, const QImage& thumbnail);

private:
    QThreadPool* m_pool = nullptr; // owned via Qt's parent-child tree
};

} // namespace lumine::image

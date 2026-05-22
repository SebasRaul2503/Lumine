#pragma once

#include <QListWidget>
#include <QString>
#include <QStringList>

class QImage;

namespace lumine::image {
class ThumbnailLoader;
}

namespace lumine::ui {

// A vertical filmstrip of image thumbnails.
//
// Clicking an entry asks the host to navigate to it (imageSelected); the host
// pushes the active index back through setCurrentImage(), which never echoes a
// signal — that keeps navigation a one-way data flow with no feedback loop.
// Thumbnails are decoded asynchronously and filled in as they arrive.
class ThumbnailStrip : public QListWidget {
    Q_OBJECT

public:
    explicit ThumbnailStrip(QWidget* parent = nullptr);
    ~ThumbnailStrip() override;

    // Rebuilds the strip for `paths` and queues thumbnail decoding.
    void setPaths(const QStringList& paths);

    // Highlights and scrolls to `index` without emitting imageSelected().
    void setCurrentImage(int index);

signals:
    void imageSelected(int index);

private slots:
    void onThumbnailReady(int id, const QImage& thumbnail);

private:
    image::ThumbnailLoader* m_loader = nullptr;
};

} // namespace lumine::ui

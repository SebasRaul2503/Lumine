#pragma once

#include <QList>
#include <QString>

class QImage;

namespace lumine::image {

// A single human-readable property row for the info panel.
struct ImageProperty {
    QString label;
    QString value;
};

// Gathers displayable properties of an image. File facts come from the path;
// pixel facts come from the already-decoded QImage, so nothing is decoded a
// second time. Pure logic — no UI types.
class ImageInfo {
public:
    [[nodiscard]] static QList<ImageProperty> gather(const QString& path,
                                                     const QImage& image);

    // Formats a byte count as a compact, locale-independent human string,
    // e.g. "1.4 MB". Used for the on-disk file size.
    [[nodiscard]] static QString formatBytes(qint64 bytes);
};

} // namespace lumine::image

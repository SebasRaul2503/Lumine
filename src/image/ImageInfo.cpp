#include "image/ImageInfo.hpp"

#include <QDateTime>
#include <QFileInfo>
#include <QImage>

namespace lumine::image {

QString ImageInfo::formatBytes(qint64 bytes)
{
    constexpr double kKiB = 1024.0;
    constexpr double kMiB = kKiB * 1024.0;
    constexpr double kGiB = kMiB * 1024.0;

    const double value = static_cast<double>(bytes);
    if (value >= kGiB) {
        return QStringLiteral("%1 GB").arg(QString::number(value / kGiB, 'f', 2));
    }
    if (value >= kMiB) {
        return QStringLiteral("%1 MB").arg(QString::number(value / kMiB, 'f', 1));
    }
    if (value >= kKiB) {
        return QStringLiteral("%1 KB").arg(QString::number(value / kKiB, 'f', 1));
    }
    return QStringLiteral("%1 bytes").arg(bytes);
}

QList<ImageProperty> ImageInfo::gather(const QString& path, const QImage& image)
{
    QList<ImageProperty> properties;

    const QFileInfo info(path);
    if (info.exists()) {
        properties.append({QStringLiteral("File"), info.fileName()});
        properties.append({QStringLiteral("Folder"), info.absolutePath()});
        properties.append({QStringLiteral("Size"), formatBytes(info.size())});
        properties.append(
            {QStringLiteral("Modified"),
             info.lastModified().toString(QStringLiteral("yyyy-MM-dd HH:mm"))});
        properties.append({QStringLiteral("Type"), info.suffix().toUpper()});
    }

    if (!image.isNull()) {
        properties.append(
            {QStringLiteral("Dimensions"),
             QStringLiteral("%1 × %2 px").arg(image.width()).arg(image.height())});

        const double megapixels =
            static_cast<double>(image.width()) * image.height() / 1.0e6;
        properties.append(
            {QStringLiteral("Resolution"),
             QStringLiteral("%1 MP").arg(QString::number(megapixels, 'f', 1))});

        properties.append(
            {QStringLiteral("Depth"), QStringLiteral("%1-bit").arg(image.depth())});
        properties.append({QStringLiteral("Alpha"), image.hasAlphaChannel()
                                                        ? QStringLiteral("Yes")
                                                        : QStringLiteral("No")});
    }
    return properties;
}

} // namespace lumine::image

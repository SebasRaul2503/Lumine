#include "image/ImageLoader.hpp"

#include "core/Logging.hpp"

#include <QFileInfo>
#include <QImageReader>

namespace lumine::image {

LoadResult ImageLoader::load(const QString& path)
{
    LoadResult result;

    const QFileInfo info(path);
    if (!info.exists() || !info.isFile()) {
        result.error = LoadError::FileNotFound;
        result.message = QStringLiteral("File not found: %1").arg(path);
        qCWarning(lcImage) << result.message;
        return result;
    }

    QImageReader reader(path);
    reader.setAutoTransform(true); // apply EXIF orientation during decode

    if (!reader.canRead()) {
        result.error = LoadError::UnsupportedFormat;
        result.message = QStringLiteral("Unsupported or unreadable format: %1").arg(path);
        qCWarning(lcImage) << result.message;
        return result;
    }

    result.image = reader.read();
    if (result.image.isNull()) {
        result.error = LoadError::DecodeFailed;
        result.message =
            QStringLiteral("Could not decode %1 (%2)").arg(path, reader.errorString());
        qCWarning(lcImage) << result.message;
        return result;
    }

    qCInfo(lcImage) << "Loaded" << path << result.image.size();
    return result;
}

QStringList ImageLoader::supportedSuffixes()
{
    const QList<QByteArray> formats = QImageReader::supportedImageFormats();

    QStringList suffixes;
    suffixes.reserve(formats.size());
    for (const QByteArray& format : formats) {
        suffixes.append(QString::fromLatin1(format).toLower());
    }
    return suffixes;
}

bool ImageLoader::isSupported(const QString& path)
{
    const QString suffix = QFileInfo(path).suffix().toLower();
    return supportedSuffixes().contains(suffix);
}

QImage ImageLoader::loadThumbnail(const QString& path, const QSize& maxSize)
{
    QImageReader reader(path);
    reader.setAutoTransform(true);
    if (!reader.canRead()) {
        return {};
    }

    // When the format reports its size up front, ask the decoder to produce
    // the reduced resolution directly — far cheaper than a full decode.
    const QSize original = reader.size();
    if (original.isValid() && !original.isEmpty()) {
        reader.setScaledSize(original.scaled(maxSize, Qt::KeepAspectRatio));
    }

    QImage thumbnail = reader.read();
    if (thumbnail.isNull()) {
        return {};
    }

    // Guarantee the result fits maxSize even when the format hid its size.
    if (thumbnail.width() > maxSize.width() || thumbnail.height() > maxSize.height()) {
        thumbnail =
            thumbnail.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    return thumbnail;
}

} // namespace lumine::image

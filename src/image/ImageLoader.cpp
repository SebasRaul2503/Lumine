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

} // namespace lumine::image

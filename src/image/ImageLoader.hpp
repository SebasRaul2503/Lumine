#pragma once

#include <QImage>
#include <QSize>
#include <QString>
#include <QStringList>

namespace lumine::image {

// Why an image failed to load. Distinguishing the cases lets the UI present
// an actionable message instead of a generic error.
enum class LoadError {
    None,
    FileNotFound,
    UnsupportedFormat,
    DecodeFailed,
};

// Outcome of a load attempt: either a valid image or a described failure.
struct LoadResult {
    QImage image;
    LoadError error = LoadError::None;
    QString message;

    [[nodiscard]] bool ok() const noexcept { return error == LoadError::None; }
};

// Decoded images are capped to this many pixels on their longer side. The
// value sits far above any normal photograph, so it only bounds
// pathologically large images and protects Lumine from exhausting memory.
inline constexpr int kDefaultMaxLongSide = 12000;

// Stateless, synchronous decoding of a single image file. Stateless and free
// of UI types so it can be called unchanged from a worker thread.
class ImageLoader {
public:
    // Decodes the image at `path`, honouring EXIF orientation. An image whose
    // longer side exceeds `maxLongSide` is decoded down-scaled to that bound
    // (pass a non-positive value to disable the cap). Never throws; failures
    // are reported through LoadResult::error.
    [[nodiscard]] static LoadResult load(const QString& path,
                                         int maxLongSide = kDefaultMaxLongSide);

    // Lower-case file suffixes (without a leading dot) that Qt can decode in
    // this build, e.g. {"png", "jpg", "webp"}.
    [[nodiscard]] static QStringList supportedSuffixes();

    // True when `path` carries a suffix Qt advertises decoding support for.
    [[nodiscard]] static bool isSupported(const QString& path);

    // Decodes a down-scaled thumbnail of `path` that fits within `maxSize`
    // while preserving aspect ratio. Decodes directly at the reduced size
    // when the format reports its dimensions up front, avoiding a full-size
    // decode. Returns a null QImage on any failure.
    [[nodiscard]] static QImage loadThumbnail(const QString& path, const QSize& maxSize);
};

} // namespace lumine::image

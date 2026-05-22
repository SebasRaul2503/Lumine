#pragma once

#include <QImage>
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

// Stateless, synchronous decoding of a single image file.
//
// Iteration 1 decodes on the calling thread. Asynchronous, cancellable
// loading is introduced in Phase 2 — see docs/roadmap.md.
class ImageLoader {
public:
    // Decodes the image at `path`, honouring EXIF orientation. Never throws;
    // failures are reported through LoadResult::error.
    [[nodiscard]] static LoadResult load(const QString& path);

    // Lower-case file suffixes (without a leading dot) that Qt can decode in
    // this build, e.g. {"png", "jpg", "webp"}.
    [[nodiscard]] static QStringList supportedSuffixes();

    // True when `path` carries a suffix Qt advertises decoding support for.
    [[nodiscard]] static bool isSupported(const QString& path);
};

} // namespace lumine::image

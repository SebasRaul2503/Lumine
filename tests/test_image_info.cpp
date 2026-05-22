#include <catch2/catch_test_macros.hpp>

#include "image/ImageInfo.hpp"

#include <QDir>
#include <QImage>
#include <QList>
#include <QString>
#include <QTemporaryDir>

using lumine::image::ImageInfo;
using lumine::image::ImageProperty;

namespace {

QString valueOf(const QList<ImageProperty>& properties, const QString& label)
{
    for (const ImageProperty& property : properties) {
        if (property.label == label) {
            return property.value;
        }
    }
    return {};
}

} // namespace

TEST_CASE("ImageInfo::formatBytes scales units", "[info]")
{
    CHECK(ImageInfo::formatBytes(0) == QStringLiteral("0 bytes"));
    CHECK(ImageInfo::formatBytes(512) == QStringLiteral("512 bytes"));
    CHECK(ImageInfo::formatBytes(1536) == QStringLiteral("1.5 KB"));
    CHECK(ImageInfo::formatBytes(5 * 1024 * 1024) == QStringLiteral("5.0 MB"));
}

TEST_CASE("ImageInfo::gather reports image and file facts", "[info]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    QImage image(200, 100, QImage::Format_RGB32);
    image.fill(Qt::red);
    const QString path = QDir(tmp.path()).filePath(QStringLiteral("photo.png"));
    REQUIRE(image.save(path, "PNG"));

    const QList<ImageProperty> props = ImageInfo::gather(path, image);

    REQUIRE_FALSE(props.isEmpty());
    CHECK(valueOf(props, QStringLiteral("File")) == QStringLiteral("photo.png"));
    CHECK(valueOf(props, QStringLiteral("Dimensions")) == QStringLiteral("200 × 100 px"));
    CHECK(valueOf(props, QStringLiteral("Depth")) == QStringLiteral("32-bit"));
    CHECK(valueOf(props, QStringLiteral("Type")) == QStringLiteral("PNG"));
}

TEST_CASE("ImageInfo::gather tolerates a null image and missing file", "[info][edge]")
{
    const QList<ImageProperty> props =
        ImageInfo::gather(QStringLiteral("/no/such/file.png"), QImage());
    CHECK(props.isEmpty());
}

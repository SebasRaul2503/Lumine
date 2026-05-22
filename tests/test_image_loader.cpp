#include <catch2/catch_test_macros.hpp>

#include "image/ImageLoader.hpp"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>

using lumine::image::ImageLoader;
using lumine::image::LoadError;

namespace {

// Writes a small, valid PNG and returns its path.
QString writeValidPng(const QDir& dir)
{
    QImage image(64, 48, QImage::Format_ARGB32);
    image.fill(Qt::darkCyan);
    const QString path = dir.filePath(QStringLiteral("valid.png"));
    REQUIRE(image.save(path, "PNG"));
    return path;
}

// Writes a file with a valid PNG signature followed by garbage: the format is
// recognised but decoding must fail.
QString writeCorruptPng(const QDir& dir)
{
    const QString path = dir.filePath(QStringLiteral("corrupt.png"));
    QFile file(path);
    REQUIRE(file.open(QIODevice::WriteOnly));
    file.write(QByteArray::fromHex("89504E470D0A1A0A"));
    file.write(QByteArray(128, '\x7F'));
    file.close();
    return path;
}

// Writes a text file with an extension Qt has no handler for.
QString writeUnsupportedFile(const QDir& dir)
{
    const QString path = dir.filePath(QStringLiteral("notes.xyz"));
    QFile file(path);
    REQUIRE(file.open(QIODevice::WriteOnly));
    file.write("this is plainly not an image");
    file.close();
    return path;
}

} // namespace

TEST_CASE("ImageLoader decodes a valid image", "[loader]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QString path = writeValidPng(QDir(tmp.path()));

    const auto result = ImageLoader::load(path);

    REQUIRE(result.ok());
    CHECK(result.error == LoadError::None);
    CHECK(result.image.width() == 64);
    CHECK(result.image.height() == 48);
    CHECK(result.message.isEmpty());
}

TEST_CASE("ImageLoader reports a missing file", "[loader][error]")
{
    const auto result =
        ImageLoader::load(QStringLiteral("/nonexistent/lumine-does-not-exist.png"));

    REQUIRE_FALSE(result.ok());
    CHECK(result.error == LoadError::FileNotFound);
    CHECK(result.image.isNull());
    CHECK_FALSE(result.message.isEmpty());
}

TEST_CASE("ImageLoader rejects a corrupt image", "[loader][error]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QString path = writeCorruptPng(QDir(tmp.path()));

    const auto result = ImageLoader::load(path);

    REQUIRE_FALSE(result.ok());
    CHECK(result.image.isNull());
    // A truncated PNG fails either the format probe or the decode step.
    CHECK((result.error == LoadError::DecodeFailed ||
           result.error == LoadError::UnsupportedFormat));
}

TEST_CASE("ImageLoader rejects unsupported content", "[loader][error]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QString path = writeUnsupportedFile(QDir(tmp.path()));

    const auto result = ImageLoader::load(path);

    REQUIRE_FALSE(result.ok());
    CHECK(result.image.isNull());
}

TEST_CASE("ImageLoader exposes supported suffixes", "[loader]")
{
    const QStringList suffixes = ImageLoader::supportedSuffixes();

    REQUIRE_FALSE(suffixes.isEmpty());
    // PNG support is built into Qt and therefore always present.
    CHECK(suffixes.contains(QStringLiteral("png")));
}

TEST_CASE("ImageLoader recognises supported and unsupported suffixes", "[loader]")
{
    CHECK(ImageLoader::isSupported(QStringLiteral("/some/where/photo.png")));
    CHECK(ImageLoader::isSupported(QStringLiteral("UPPER.PNG")));
    CHECK_FALSE(ImageLoader::isSupported(QStringLiteral("archive.tar.gz")));
    CHECK_FALSE(ImageLoader::isSupported(QStringLiteral("noextension")));
}

TEST_CASE("ImageLoader::loadThumbnail produces a bounded thumbnail",
          "[loader][thumbnail]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    QImage source(400, 300, QImage::Format_RGB32);
    source.fill(Qt::blue);
    const QString path = QDir(tmp.path()).filePath(QStringLiteral("big.png"));
    REQUIRE(source.save(path, "PNG"));

    const QImage thumb = ImageLoader::loadThumbnail(path, QSize(100, 100));

    REQUIRE_FALSE(thumb.isNull());
    CHECK(thumb.width() <= 100);
    CHECK(thumb.height() <= 100);
    // The 4:3 aspect ratio is preserved: 400x300 fits 100x100 as 100x75.
    CHECK(thumb.width() == 100);
    CHECK(thumb.height() == 75);
}

TEST_CASE("ImageLoader::loadThumbnail returns null for a missing file",
          "[loader][thumbnail][error]")
{
    const QImage thumb = ImageLoader::loadThumbnail(
        QStringLiteral("/no/such/lumine-thumbnail.png"), QSize(64, 64));
    CHECK(thumb.isNull());
}

TEST_CASE("ImageLoader caps the decode size of an oversized image", "[loader][huge]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    QImage source(400, 300, QImage::Format_RGB32);
    source.fill(Qt::green);
    const QString path = QDir(tmp.path()).filePath(QStringLiteral("large.png"));
    REQUIRE(source.save(path, "PNG"));

    // A deliberately tiny cap stands in for the real huge-image threshold.
    const auto result = ImageLoader::load(path, 100);

    REQUIRE(result.ok());
    CHECK(result.image.width() <= 100);
    CHECK(result.image.height() <= 100);
    // The 4:3 aspect ratio survives the cap: 400x300 becomes 100x75.
    CHECK(result.image.width() == 100);
    CHECK(result.image.height() == 75);
}

TEST_CASE("ImageLoader leaves an image below the cap untouched", "[loader]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    QImage source(120, 90, QImage::Format_RGB32);
    source.fill(Qt::green);
    const QString path = QDir(tmp.path()).filePath(QStringLiteral("small.png"));
    REQUIRE(source.save(path, "PNG"));

    const auto result = ImageLoader::load(path, 100000);

    REQUIRE(result.ok());
    CHECK(result.image.width() == 120);
    CHECK(result.image.height() == 90);
}

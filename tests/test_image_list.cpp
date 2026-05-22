#include <catch2/catch_test_macros.hpp>

#include "image/ImageList.hpp"

#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QString>
#include <QTemporaryDir>

using lumine::image::ImageList;

namespace {

// Writes a tiny valid PNG named `name` into `dir`.
void writePng(const QDir& dir, const QString& name)
{
    QImage image(8, 8, QImage::Format_RGB32);
    image.fill(Qt::black);
    REQUIRE(image.save(dir.filePath(name), "PNG"));
}

QString fileName(const QString& path)
{
    return QFileInfo(path).fileName();
}

} // namespace

TEST_CASE("ImageList scans a directory and selects the opened file", "[list]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QDir dir(tmp.path());
    writePng(dir, QStringLiteral("b.png"));
    writePng(dir, QStringLiteral("a.png"));
    writePng(dir, QStringLiteral("c.png"));

    ImageList list;
    list.openFromFile(dir.filePath(QStringLiteral("b.png")));

    REQUIRE(list.count() == 3);
    // Sorted case-insensitively: a, b, c — so b.png lands at index 1.
    CHECK(list.currentIndex() == 1);
    CHECK(fileName(list.current()) == QStringLiteral("b.png"));
}

TEST_CASE("ImageList navigation wraps around the ends", "[list]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QDir dir(tmp.path());
    writePng(dir, QStringLiteral("a.png"));
    writePng(dir, QStringLiteral("b.png"));

    ImageList list;
    list.openFromFile(dir.filePath(QStringLiteral("a.png")));
    REQUIRE(list.currentIndex() == 0);

    CHECK(fileName(list.next()) == QStringLiteral("b.png"));
    CHECK(fileName(list.next()) == QStringLiteral("a.png"));     // wrapped
    CHECK(fileName(list.previous()) == QStringLiteral("b.png")); // wrapped back
}

TEST_CASE("ImageList peek wraps in both directions", "[list]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QDir dir(tmp.path());
    writePng(dir, QStringLiteral("a.png"));
    writePng(dir, QStringLiteral("b.png"));
    writePng(dir, QStringLiteral("c.png"));

    ImageList list;
    list.openFromFile(dir.filePath(QStringLiteral("a.png"))); // index 0

    CHECK(fileName(list.peek(1)) == QStringLiteral("b.png"));
    CHECK(fileName(list.peek(2)) == QStringLiteral("c.png"));
    CHECK(fileName(list.peek(3)) == QStringLiteral("a.png"));  // wrap forward
    CHECK(fileName(list.peek(-1)) == QStringLiteral("c.png")); // wrap backward
}

TEST_CASE("ImageList handles an empty directory", "[list][edge]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());

    ImageList list;
    list.openDirectory(tmp.path());

    CHECK(list.isEmpty());
    CHECK(list.count() == 0);
    CHECK(list.currentIndex() == -1);
    CHECK(list.current().isEmpty());
    CHECK(list.next().isEmpty());
    CHECK(list.peek(1).isEmpty());
}

TEST_CASE("ImageList setCurrentIndex rejects out-of-range values", "[list][edge]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QDir dir(tmp.path());
    writePng(dir, QStringLiteral("a.png"));
    writePng(dir, QStringLiteral("b.png"));

    ImageList list;
    list.openFromFile(dir.filePath(QStringLiteral("a.png")));

    CHECK(list.setCurrentIndex(1));
    CHECK(list.currentIndex() == 1);
    CHECK_FALSE(list.setCurrentIndex(2));
    CHECK_FALSE(list.setCurrentIndex(-1));
    CHECK(list.currentIndex() == 1); // unchanged after rejected calls
}

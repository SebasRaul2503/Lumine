#include <catch2/catch_test_macros.hpp>

#include "core/ImageCache.hpp"

#include <QImage>

using lumine::core::ImageCache;

namespace {

// A solid RGB32 image; its byte size is deterministic and used to size the
// cache budget precisely in the eviction tests.
QImage solidImage(int width, int height)
{
    QImage image(width, height, QImage::Format_RGB32);
    image.fill(Qt::gray);
    return image;
}

constexpr qint64 kLargeBudget = 64LL * 1024 * 1024;

} // namespace

TEST_CASE("ImageCache stores and returns an image", "[cache]")
{
    ImageCache cache(kLargeBudget);
    cache.insert(QStringLiteral("/a.png"), solidImage(20, 20));

    REQUIRE(cache.contains(QStringLiteral("/a.png")));
    CHECK(cache.count() == 1);

    const QImage got = cache.get(QStringLiteral("/a.png"));
    CHECK(got.width() == 20);
    CHECK(got.height() == 20);
}

TEST_CASE("ImageCache returns null for a missing key", "[cache][edge]")
{
    ImageCache cache(kLargeBudget);
    CHECK_FALSE(cache.contains(QStringLiteral("/missing.png")));
    CHECK(cache.get(QStringLiteral("/missing.png")).isNull());
}

TEST_CASE("ImageCache evicts the least-recently-used entry", "[cache]")
{
    const QImage image = solidImage(100, 100);
    const qint64 each = static_cast<qint64>(image.sizeInBytes());
    ImageCache cache(each * 2); // budget holds exactly two images

    cache.insert(QStringLiteral("/a.png"), image);
    cache.insert(QStringLiteral("/b.png"), image);
    cache.insert(QStringLiteral("/c.png"), image); // evicts the LRU entry

    CHECK_FALSE(cache.contains(QStringLiteral("/a.png")));
    CHECK(cache.contains(QStringLiteral("/b.png")));
    CHECK(cache.contains(QStringLiteral("/c.png")));
    CHECK(cache.count() == 2);
}

TEST_CASE("ImageCache::get refreshes recency", "[cache]")
{
    const QImage image = solidImage(100, 100);
    const qint64 each = static_cast<qint64>(image.sizeInBytes());
    ImageCache cache(each * 2);

    cache.insert(QStringLiteral("/a.png"), image);
    cache.insert(QStringLiteral("/b.png"), image);
    const QImage refreshed = cache.get(QStringLiteral("/a.png")); // a now MRU
    REQUIRE_FALSE(refreshed.isNull());
    cache.insert(QStringLiteral("/c.png"), image); // evicts b, not a

    CHECK(cache.contains(QStringLiteral("/a.png")));
    CHECK_FALSE(cache.contains(QStringLiteral("/b.png")));
    CHECK(cache.contains(QStringLiteral("/c.png")));
}

TEST_CASE("ImageCache refuses an image larger than the budget", "[cache][edge]")
{
    const QImage tiny = solidImage(10, 10);
    ImageCache cache(static_cast<qint64>(tiny.sizeInBytes()));

    cache.insert(QStringLiteral("/big.png"), solidImage(200, 200));

    CHECK_FALSE(cache.contains(QStringLiteral("/big.png")));
    CHECK(cache.usedBytes() == 0);
}

TEST_CASE("ImageCache tracks used bytes across insert and remove", "[cache]")
{
    const QImage image = solidImage(50, 50);
    ImageCache cache(kLargeBudget);

    cache.insert(QStringLiteral("/a.png"), image);
    CHECK(cache.usedBytes() == static_cast<qint64>(image.sizeInBytes()));

    cache.remove(QStringLiteral("/a.png"));
    CHECK(cache.usedBytes() == 0);
    CHECK(cache.count() == 0);
}

TEST_CASE("ImageCache shrinking the budget evicts to fit", "[cache][edge]")
{
    const QImage image = solidImage(100, 100);
    const qint64 each = static_cast<qint64>(image.sizeInBytes());
    ImageCache cache(each * 4);

    cache.insert(QStringLiteral("/a.png"), image);
    cache.insert(QStringLiteral("/b.png"), image);
    cache.insert(QStringLiteral("/c.png"), image);
    REQUIRE(cache.count() == 3);

    cache.setBudgetBytes(each); // only the most-recent entry survives
    CHECK(cache.count() == 1);
    CHECK(cache.contains(QStringLiteral("/c.png")));
}

TEST_CASE("ImageCache clear empties everything", "[cache]")
{
    ImageCache cache(kLargeBudget);
    cache.insert(QStringLiteral("/a.png"), solidImage(30, 30));
    cache.insert(QStringLiteral("/b.png"), solidImage(30, 30));

    cache.clear();

    CHECK(cache.count() == 0);
    CHECK(cache.usedBytes() == 0);
    CHECK_FALSE(cache.contains(QStringLiteral("/a.png")));
}

TEST_CASE("ImageCache ignores null images", "[cache][edge]")
{
    ImageCache cache(kLargeBudget);
    cache.insert(QStringLiteral("/null.png"), QImage());

    CHECK_FALSE(cache.contains(QStringLiteral("/null.png")));
    CHECK(cache.count() == 0);
}

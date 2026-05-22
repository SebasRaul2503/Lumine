#include <catch2/catch_test_macros.hpp>

#include "image/AsyncImageLoader.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QEventLoop>
#include <QImage>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <QTimer>

using lumine::image::AsyncImageLoader;

namespace {

// Returns a process-wide QCoreApplication, creating (and leaking) one on first
// use. AsyncImageLoader needs a running event loop to deliver queued results.
QCoreApplication& testApp()
{
    static int argc = 1;
    static char arg0[] = "lumine_tests";
    static char* argv[] = {arg0, nullptr};
    static QCoreApplication* app = QCoreApplication::instance() != nullptr
                                       ? QCoreApplication::instance()
                                       : new QCoreApplication(argc, argv);
    return *app;
}

// Spins the event loop until `loop` quits or the safety timeout fires.
void runWithTimeout(QEventLoop& loop, int timeoutMs = 5000)
{
    QTimer::singleShot(timeoutMs, &loop, &QEventLoop::quit);
    loop.exec();
}

QString writeImage(const QDir& dir, const QString& name, int w, int h)
{
    QImage image(w, h, QImage::Format_RGB32);
    image.fill(Qt::magenta);
    const QString path = dir.filePath(name);
    REQUIRE(image.save(path, "PNG"));
    return path;
}

} // namespace

TEST_CASE("AsyncImageLoader delivers a decoded image", "[async]")
{
    testApp();
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QString path = writeImage(QDir(tmp.path()), QStringLiteral("a.png"), 50, 40);

    AsyncImageLoader loader;
    QImage received;
    QString receivedPath;
    bool failed = false;

    QEventLoop loop;
    QObject::connect(&loader, &AsyncImageLoader::loaded, &loop,
                     [&](const QString& p, const QImage& img) {
                         receivedPath = p;
                         received = img;
                         loop.quit();
                     });
    QObject::connect(&loader, &AsyncImageLoader::failed, &loop,
                     [&](const QString&, const QString&) {
                         failed = true;
                         loop.quit();
                     });

    loader.request(path);
    runWithTimeout(loop);

    REQUIRE_FALSE(failed);
    CHECK(receivedPath == path);
    CHECK(received.width() == 50);
    CHECK(received.height() == 40);
}

TEST_CASE("AsyncImageLoader reports failure for a missing file", "[async]")
{
    testApp();
    AsyncImageLoader loader;
    bool failed = false;

    QEventLoop loop;
    QObject::connect(&loader, &AsyncImageLoader::failed, &loop,
                     [&](const QString&, const QString&) {
                         failed = true;
                         loop.quit();
                     });
    QObject::connect(&loader, &AsyncImageLoader::loaded, &loop,
                     [&](const QString&, const QImage&) { loop.quit(); });

    loader.request(QStringLiteral("/no/such/lumine-async-missing.png"));
    runWithTimeout(loop);

    CHECK(failed);
}

TEST_CASE("AsyncImageLoader drops a superseded request", "[async][edge]")
{
    testApp();
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QDir dir(tmp.path());
    const QString first = writeImage(dir, QStringLiteral("first.png"), 30, 30);
    const QString second = writeImage(dir, QStringLiteral("second.png"), 60, 60);

    AsyncImageLoader loader;
    QStringList delivered;

    QEventLoop loop;
    QObject::connect(&loader, &AsyncImageLoader::loaded, &loop,
                     [&](const QString& p, const QImage&) {
                         delivered.append(p);
                         loop.quit();
                     });

    // Two back-to-back requests: the first is superseded before delivery.
    loader.request(first);
    loader.request(second);
    runWithTimeout(loop);

    CHECK_FALSE(delivered.contains(first));
    CHECK(delivered.contains(second));
}

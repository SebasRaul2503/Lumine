#include <catch2/catch_test_macros.hpp>

#include "core/ScopedTimer.hpp"

#include <QThread>

using lumine::core::ScopedTimer;

TEST_CASE("ScopedTimer reports non-negative elapsed time", "[perf]")
{
    const ScopedTimer timer(QStringLiteral("unit-test"));
    CHECK(timer.elapsedMs() >= 0);
}

TEST_CASE("ScopedTimer elapsed time never decreases", "[perf]")
{
    const ScopedTimer timer(QStringLiteral("unit-test"));
    const qint64 first = timer.elapsedMs();
    QThread::msleep(2);
    const qint64 second = timer.elapsedMs();

    CHECK(second >= first);
}

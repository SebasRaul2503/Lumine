#pragma once

#include <QElapsedTimer>
#include <QString>

namespace lumine::core {

// An RAII stopwatch. It starts on construction and, on destruction, logs the
// wall-clock duration of its scope to the "lumine.perf" logging category.
//
// Lightweight profiling instrumentation: it costs nothing visible unless the
// category is enabled with QT_LOGGING_RULES="lumine.perf=true".
class ScopedTimer {
public:
    explicit ScopedTimer(QString label);
    ~ScopedTimer();

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
    ScopedTimer(ScopedTimer&&) = delete;
    ScopedTimer& operator=(ScopedTimer&&) = delete;

    // Milliseconds elapsed since construction.
    [[nodiscard]] qint64 elapsedMs() const;

private:
    QString m_label;
    QElapsedTimer m_timer;
};

} // namespace lumine::core

#pragma once

#include <QHash>
#include <QImage>
#include <QList>
#include <QString>

namespace lumine::core {

// A least-recently-used cache of decoded images, bounded by a total byte
// budget. Pure logic: no threading and no I/O. Callers must use it from a
// single thread — Lumine drives it from the UI thread.
//
// Recency is tracked with a small QList (front = least recent). Image caches
// hold only a few dozen entries, so the linear updates are negligible and
// keep the implementation obvious.
class ImageCache {
public:
    explicit ImageCache(qint64 budgetBytes);

    // Inserts or replaces the image for `path` and marks it most-recently
    // used. Null images, and images larger than the whole budget, are
    // ignored. Inserting may evict other entries to stay within budget.
    void insert(const QString& path, const QImage& image);

    // Returns the cached image for `path`, marking it most-recently used, or
    // a null QImage when absent.
    [[nodiscard]] QImage get(const QString& path);

    // Reports whether `path` is cached, without affecting recency.
    [[nodiscard]] bool contains(const QString& path) const;

    void remove(const QString& path);
    void clear();

    [[nodiscard]] qint64 budgetBytes() const noexcept;
    [[nodiscard]] qint64 usedBytes() const noexcept;
    [[nodiscard]] int count() const noexcept;

    // Changes the budget, evicting least-recently-used entries to fit.
    void setBudgetBytes(qint64 budgetBytes);

private:
    struct Entry {
        QImage image;
        qint64 bytes = 0;
    };

    void touch(const QString& path);
    void evictUntilWithinBudget();

    QHash<QString, Entry> m_entries;
    QList<QString> m_lru; // front = least recent, back = most recent
    qint64 m_budgetBytes = 0;
    qint64 m_usedBytes = 0;
};

} // namespace lumine::core

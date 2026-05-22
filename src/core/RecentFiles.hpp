#pragma once

#include <QString>
#include <QStringList>

class QSettings;

namespace lumine::core {

// A bounded, most-recent-first list of recently opened file paths, with
// de-duplication.
//
// The list logic is pure and unit-testable; persistence is delegated to a
// caller-supplied QSettings through load() and save(), so the class never
// reaches for global state itself.
class RecentFiles {
public:
    explicit RecentFiles(int maxEntries = 12);

    // Moves `path` to the front, dropping any earlier occurrence, then trims
    // the list to maxEntries. Empty paths are ignored.
    void add(const QString& path);

    [[nodiscard]] QStringList list() const;
    [[nodiscard]] int maxEntries() const noexcept;
    void clear();

    // Reads the list from `settings` (and trims it to maxEntries).
    void load(QSettings& settings);

    // Writes the list to `settings`.
    void save(QSettings& settings) const;

private:
    void trim();

    QStringList m_paths;
    int m_maxEntries;
};

} // namespace lumine::core

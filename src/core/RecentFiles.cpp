#include "core/RecentFiles.hpp"

#include <QSettings>

namespace lumine::core {

namespace {
// QSettings key under which the recent list is stored.
constexpr auto kSettingsKey = "recentFiles";
} // namespace

RecentFiles::RecentFiles(int maxEntries) : m_maxEntries(maxEntries > 0 ? maxEntries : 1)
{
}

void RecentFiles::add(const QString& path)
{
    if (path.isEmpty()) {
        return;
    }
    m_paths.removeAll(path);
    m_paths.prepend(path);
    trim();
}

QStringList RecentFiles::list() const
{
    return m_paths;
}

int RecentFiles::maxEntries() const noexcept
{
    return m_maxEntries;
}

void RecentFiles::clear()
{
    m_paths.clear();
}

void RecentFiles::load(QSettings& settings)
{
    m_paths = settings.value(QString::fromLatin1(kSettingsKey)).toStringList();
    trim();
}

void RecentFiles::save(QSettings& settings) const
{
    settings.setValue(QString::fromLatin1(kSettingsKey), m_paths);
}

void RecentFiles::trim()
{
    while (m_paths.size() > m_maxEntries) {
        m_paths.removeLast();
    }
}

} // namespace lumine::core

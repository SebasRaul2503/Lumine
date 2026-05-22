#include "core/ImageCache.hpp"

#include "core/Logging.hpp"

namespace lumine::core {

ImageCache::ImageCache(qint64 budgetBytes)
    : m_budgetBytes(budgetBytes > 0 ? budgetBytes : 0)
{
}

void ImageCache::insert(const QString& path, const QImage& image)
{
    if (path.isEmpty() || image.isNull()) {
        return;
    }

    remove(path); // replace any existing entry for this path

    const qint64 bytes = static_cast<qint64>(image.sizeInBytes());
    if (bytes > m_budgetBytes) {
        qCDebug(lcImage) << "Image too large to cache:" << path << bytes << "bytes";
        return;
    }

    m_entries.insert(path, Entry{image, bytes});
    m_lru.append(path);
    m_usedBytes += bytes;
    evictUntilWithinBudget();
}

QImage ImageCache::get(const QString& path)
{
    const auto it = m_entries.constFind(path);
    if (it == m_entries.constEnd()) {
        return {};
    }
    touch(path);
    return it.value().image;
}

bool ImageCache::contains(const QString& path) const
{
    return m_entries.contains(path);
}

void ImageCache::remove(const QString& path)
{
    const auto it = m_entries.find(path);
    if (it == m_entries.end()) {
        return;
    }
    m_usedBytes -= it.value().bytes;
    m_entries.erase(it);
    m_lru.removeOne(path);
}

void ImageCache::clear()
{
    m_entries.clear();
    m_lru.clear();
    m_usedBytes = 0;
}

qint64 ImageCache::budgetBytes() const noexcept
{
    return m_budgetBytes;
}

qint64 ImageCache::usedBytes() const noexcept
{
    return m_usedBytes;
}

int ImageCache::count() const noexcept
{
    return static_cast<int>(m_entries.size());
}

void ImageCache::setBudgetBytes(qint64 budgetBytes)
{
    m_budgetBytes = budgetBytes > 0 ? budgetBytes : 0;
    evictUntilWithinBudget();
}

void ImageCache::touch(const QString& path)
{
    m_lru.removeOne(path);
    m_lru.append(path);
}

void ImageCache::evictUntilWithinBudget()
{
    while (m_usedBytes > m_budgetBytes && !m_lru.isEmpty()) {
        const QString victim = m_lru.takeFirst();
        const auto it = m_entries.find(victim);
        if (it != m_entries.end()) {
            m_usedBytes -= it.value().bytes;
            m_entries.erase(it);
            qCDebug(lcImage) << "Cache evicted" << victim;
        }
    }
}

} // namespace lumine::core

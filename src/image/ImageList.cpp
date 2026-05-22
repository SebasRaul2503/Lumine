#include "image/ImageList.hpp"

#include "core/Logging.hpp"
#include "image/ImageLoader.hpp"

#include <QDir>
#include <QFileInfo>

namespace lumine::image {

void ImageList::openFromFile(const QString& path)
{
    const QFileInfo info(path);
    openDirectory(info.absolutePath());

    const int index = static_cast<int>(m_paths.indexOf(info.absoluteFilePath()));
    if (index >= 0) {
        m_currentIndex = index;
    }
}

void ImageList::openDirectory(const QString& directory)
{
    clear();

    const QStringList suffixes = ImageLoader::supportedSuffixes();
    QStringList nameFilters;
    nameFilters.reserve(suffixes.size());
    for (const QString& suffix : suffixes) {
        nameFilters.append(QStringLiteral("*.%1").arg(suffix));
    }

    const QDir dir(directory);
    const QFileInfoList entries =
        dir.entryInfoList(nameFilters, QDir::Files | QDir::Readable,
                          QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);

    m_paths.reserve(entries.size());
    for (const QFileInfo& entry : entries) {
        m_paths.append(entry.absoluteFilePath());
    }

    m_currentIndex = m_paths.isEmpty() ? -1 : 0;
    qCInfo(lcImage) << "Image list:" << count() << "file(s) in" << directory;
}

void ImageList::clear()
{
    m_paths.clear();
    m_currentIndex = -1;
}

bool ImageList::isEmpty() const noexcept
{
    return m_paths.isEmpty();
}

int ImageList::count() const noexcept
{
    return static_cast<int>(m_paths.size());
}

int ImageList::currentIndex() const noexcept
{
    return m_currentIndex;
}

QString ImageList::current() const
{
    if (m_currentIndex < 0 || m_currentIndex >= count()) {
        return {};
    }
    return m_paths.at(m_currentIndex);
}

const QStringList& ImageList::paths() const noexcept
{
    return m_paths;
}

QString ImageList::next()
{
    if (m_paths.isEmpty()) {
        return {};
    }
    m_currentIndex = (m_currentIndex + 1) % count();
    return current();
}

QString ImageList::previous()
{
    if (m_paths.isEmpty()) {
        return {};
    }
    m_currentIndex = (m_currentIndex - 1 + count()) % count();
    return current();
}

bool ImageList::setCurrentIndex(int index)
{
    if (index < 0 || index >= count()) {
        return false;
    }
    m_currentIndex = index;
    return true;
}

QString ImageList::peek(int offset) const
{
    if (m_paths.isEmpty()) {
        return {};
    }
    const int wrapped = ((m_currentIndex + offset) % count() + count()) % count();
    return m_paths.at(wrapped);
}

} // namespace lumine::image

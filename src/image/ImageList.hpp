#pragma once

#include <QString>
#include <QStringList>

namespace lumine::image {

// An ordered, navigable list of image files from a single directory.
//
// Pure logic: the only I/O is the one-shot directory scan. No UI types, no
// decoding — that keeps navigation instant and the class fully unit-testable.
class ImageList {
public:
    ImageList() = default;

    // Scans the directory containing `path` and selects `path` itself when it
    // is a supported image. Entries are sorted case-insensitively by name.
    void openFromFile(const QString& path);

    // Scans `directory` for every supported image and selects the first.
    void openDirectory(const QString& directory);

    void clear();

    [[nodiscard]] bool isEmpty() const noexcept;
    [[nodiscard]] int count() const noexcept;
    [[nodiscard]] int currentIndex() const noexcept;

    // The selected path, or an empty string when the list is empty.
    [[nodiscard]] QString current() const;

    // Absolute paths of every image in the list.
    [[nodiscard]] const QStringList& paths() const noexcept;

    // Moves the selection one step, wrapping at the ends, and returns the
    // newly selected path. A no-op returning an empty string when empty.
    QString next();
    QString previous();

    // Selects `index`; returns false (leaving the selection unchanged) when
    // `index` is out of range.
    bool setCurrentIndex(int index);

    // Path `offset` positions from the current one, wrapping in both
    // directions. Empty when the list is empty. Used for preloading.
    [[nodiscard]] QString peek(int offset) const;

private:
    QStringList m_paths;
    int m_currentIndex = -1;
};

} // namespace lumine::image

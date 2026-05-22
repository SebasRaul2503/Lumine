#include <catch2/catch_test_macros.hpp>

#include "core/RecentFiles.hpp"

#include <QDir>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>

using lumine::core::RecentFiles;

TEST_CASE("RecentFiles keeps the most recent path first", "[recent]")
{
    RecentFiles recent(10);
    recent.add(QStringLiteral("/a.png"));
    recent.add(QStringLiteral("/b.png"));
    recent.add(QStringLiteral("/c.png"));

    const QStringList list = recent.list();
    REQUIRE(list.size() == 3);
    CHECK(list.at(0) == QStringLiteral("/c.png"));
    CHECK(list.at(2) == QStringLiteral("/a.png"));
}

TEST_CASE("RecentFiles de-duplicates paths", "[recent]")
{
    RecentFiles recent(10);
    recent.add(QStringLiteral("/a.png"));
    recent.add(QStringLiteral("/b.png"));
    recent.add(QStringLiteral("/a.png")); // a returns to the front

    const QStringList list = recent.list();
    REQUIRE(list.size() == 2);
    CHECK(list.at(0) == QStringLiteral("/a.png"));
    CHECK(list.at(1) == QStringLiteral("/b.png"));
}

TEST_CASE("RecentFiles caps the list at maxEntries", "[recent][edge]")
{
    RecentFiles recent(3);
    for (int i = 0; i < 6; ++i) {
        recent.add(QStringLiteral("/img%1.png").arg(i));
    }

    REQUIRE(recent.list().size() == 3);
    CHECK(recent.list().at(0) == QStringLiteral("/img5.png"));
    CHECK(recent.list().at(2) == QStringLiteral("/img3.png"));
}

TEST_CASE("RecentFiles ignores empty paths and clears", "[recent][edge]")
{
    RecentFiles recent(5);
    recent.add(QString());
    CHECK(recent.list().isEmpty());

    recent.add(QStringLiteral("/a.png"));
    recent.clear();
    CHECK(recent.list().isEmpty());
}

TEST_CASE("RecentFiles round-trips through QSettings", "[recent]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QString iniPath = QDir(tmp.path()).filePath(QStringLiteral("recent.ini"));

    {
        RecentFiles recent(5);
        recent.add(QStringLiteral("/a.png"));
        recent.add(QStringLiteral("/b.png"));
        QSettings settings(iniPath, QSettings::IniFormat);
        recent.save(settings);
        settings.sync();
    }

    RecentFiles restored(5);
    QSettings settings(iniPath, QSettings::IniFormat);
    restored.load(settings);

    REQUIRE(restored.list().size() == 2);
    CHECK(restored.list().at(0) == QStringLiteral("/b.png"));
}

TEST_CASE("RecentFiles::load trims a long stored list to maxEntries", "[recent][edge]")
{
    QTemporaryDir tmp;
    REQUIRE(tmp.isValid());
    const QString iniPath = QDir(tmp.path()).filePath(QStringLiteral("recent.ini"));

    {
        RecentFiles wide(20);
        for (int i = 0; i < 12; ++i) {
            wide.add(QStringLiteral("/img%1.png").arg(i));
        }
        QSettings settings(iniPath, QSettings::IniFormat);
        wide.save(settings);
        settings.sync();
    }

    RecentFiles narrow(4);
    QSettings settings(iniPath, QSettings::IniFormat);
    narrow.load(settings);
    CHECK(narrow.list().size() == 4);
}

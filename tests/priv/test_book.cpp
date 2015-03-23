/*
 * Copyright (c) 2015 Manuel de la Peña <mandel@themacaque.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QFileInfo>
#include <QSqlDatabase>

#include <com/chancho/system/database.h>
#include <com/chancho/system/database_factory.h>
#include "test_book.h"

void
TestBook::init() {
    BaseTestCase::init();
}

void
TestBook::cleanup() {
    BaseTestCase::cleanup();

    auto dbPath = PublicBook::databasePath();
    QFileInfo fi(dbPath);
    if (fi.exists())
        removeDir(fi.absolutePath());
}

void
TestBook::testInitDatabase() {
    auto dbPath = PublicBook::databasePath();

    // init the database and check that all the tables are present
    PublicBook::initDatabse();

    QFileInfo fi(dbPath);
    QVERIFY(fi.exists());

    // use qsql to ensure that the tables are there
    auto db = com::chancho::system::DatabaseFactory::instance()->addDatabase("QSQLITE", "TESTS");
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // once the db has been created we need to check that it has the correct version and the required tables
    auto tables = db->tables();
    QCOMPARE(tables.count(), 3);
    QVERIFY(tables.contains("Accounts", Qt::CaseInsensitive));
    QVERIFY(tables.contains("Categories", Qt::CaseInsensitive));
    db->close();
}

void
TestBook::testInitDatabaseNoPresentTables() {
    // create a database with no data, then init the database and ensure that the tables are present
    auto dbPath = PublicBook::databasePath();

    QFileInfo fi(dbPath);
    QVERIFY(!fi.exists());

    auto db = com::chancho::system::DatabaseFactory::instance()->addDatabase("QSQLITE", "TESTS");
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    db->close();
    QVERIFY(fi.exists());

    PublicBook::initDatabse();

    // assert that the tables have been created
    opened = db->open();
    QVERIFY(opened);

    // once the db has been created we need to check that it has the correct version and the required tables
    auto tables = db->tables();
    QCOMPARE(tables.count(), 3);
    QVERIFY(tables.contains("Accounts", Qt::CaseInsensitive));
    QVERIFY(tables.contains("Categories", Qt::CaseInsensitive));
    db->close();
}

void
TestBook::testInitDatabasePresentTables() {
    // simply call init twice and make sure everything goes ok
    auto dbPath = PublicBook::databasePath();

    PublicBook::initDatabse();
    PublicBook::initDatabse();

    QFileInfo fi(dbPath);
    QVERIFY(fi.exists());

    // use qsql to ensure that the tables are there
    auto db = com::chancho::system::DatabaseFactory::instance()->addDatabase("QSQLITE", "TESTS");
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // once the db has been created we need to check that it has the correct version and the required tables
    auto tables = db->tables();
    QCOMPARE(tables.count(), 3);
    QVERIFY(tables.contains("Accounts", Qt::CaseInsensitive));
    QVERIFY(tables.contains("Categories", Qt::CaseInsensitive));
    db->close();
}

QTEST_MAIN(TestBook)

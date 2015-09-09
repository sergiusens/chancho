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

#include <QDebug>
#include <QFileInfo>
#include <QSqlDatabase>

#include <com/chancho/updater.h>
#include <com/chancho/system/database.h>
#include <com/chancho/system/database_factory.h>
#include "test_upgrader.h"

namespace sys = com::chancho::system;

void
TestUpgrader::init() {
    BaseTestCase::init();
}

void
TestUpgrader::cleanup() {
    BaseTestCase::cleanup();

    auto dbPath = PublicBook::databasePath();
    QFileInfo fi(dbPath);
    if (fi.exists())
        removeDir(fi.absolutePath());
}

void
TestUpgrader::testUpgradeNoRecurrence() {
    // create a database with the basic tables (just the names, no need to add the exact fields)  and make sure it
    // is added
    chancho::Updater updater;
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();

    auto success = query->exec(chancho::Book::ACCOUNTS_TABLE);
    QVERIFY(success);

    success &= query->exec(chancho::Book::CATEGORIES_TABLE);
    QVERIFY(success);

    success &= query->exec(chancho::Book::TRANSACTION_TABLE);
    QVERIFY(success);
    db->close();

    //init the db and test that the recurrence is added
    PublicBook::initDatabse();
    QVERIFY(updater.needsUpgrade());
    updater.upgrade();

    opened = db->open();
    QVERIFY(opened);

    // once the db has been created we need to check that it has the correct version and the required tables
    auto tables = db->tables();
    QCOMPARE(tables.count(), 5);
    QVERIFY(tables.contains("Accounts", Qt::CaseInsensitive));
    QVERIFY(tables.contains("Categories", Qt::CaseInsensitive));
    QVERIFY(tables.contains("Transactions", Qt::CaseInsensitive));
    QVERIFY(tables.contains("RecurrentTransactions", Qt::CaseInsensitive));
    QVERIFY(tables.contains("RecurrentTransactionRelations", Qt::CaseInsensitive));
    db->close();
}

void
TestUpgrader::testUpgradeNoRecurrenceRelations() {
    // create a database with the basic tables (just the names, no need to add the exact fields)  and make sure it
    // is added
    chancho::Updater updater;
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();

    auto success = query->exec(chancho::Book::ACCOUNTS_TABLE);
    QVERIFY(success);

    success &= query->exec(chancho::Book::CATEGORIES_TABLE);
    QVERIFY(success);

    success &= query->exec(chancho::Book::TRANSACTION_TABLE);
    QVERIFY(success);

    success &= query->exec(chancho::Book::RECURRENT_TRANSACTION_TABLE);
    QVERIFY(success);

    db->close();

    //init the db and test that the recurrence is added
    PublicBook::initDatabse();
    QVERIFY(updater.needsUpgrade());
    updater.upgrade();

    opened = db->open();
    QVERIFY(opened);

    // once the db has been created we need to check that it has the correct version and the required tables
    auto tables = db->tables();
    qDebug() << tables;
    QCOMPARE(tables.count(), 5);
    QVERIFY(tables.contains("Accounts", Qt::CaseInsensitive));
    QVERIFY(tables.contains("Categories", Qt::CaseInsensitive));
    QVERIFY(tables.contains("Transactions", Qt::CaseInsensitive));
    QVERIFY(tables.contains("RecurrentTransactions", Qt::CaseInsensitive));
    QVERIFY(tables.contains("RecurrentTransactionRelations", Qt::CaseInsensitive));
    db->close();
}


QTEST_MAIN(TestUpgrader)

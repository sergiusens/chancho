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

#include <com/chancho/system/database_factory.h>

#include "public_account.h"
#include "public_category.h"
#include "public_transaction.h"
#include "public_recurrence.h"
#include "public_recurrent_transaction.h"

#include "test_book_recurrent_transaction.h"

namespace sys = com::chancho::system;

namespace {
    const QString SELECT_TRANSACTION_QUERY = "SELECT amount, account, category, contents, memo, startDay, startMonth, "\
        "startYear, lastDay, lastMonth, lastYear, endDay, endMonth, endYear, defaultType, numberDays, occurrences "\
        "FROM RecurrentTransactions WHERE uuid=:uuid";
}

void
TestBookRecurrentTransaction::init() {
    BaseTestCase::init();
    PublicBook::initDatabse();
}

void
TestBookRecurrentTransaction::cleanup() {
    BaseTestCase::cleanup();

    auto failed = QTest::currentTestFailed();
    auto dbPath = PublicBook::databasePath();
    if (!failed) {
        QFileInfo fi(dbPath);
        if (fi.exists())
            removeDir(fi.absolutePath());
    } else {
        qDebug() << "FAILED: " << dbPath;
        exit(1);
    }
}

void
TestBookRecurrentTransaction::testStoreAccountNotStored() {
    auto account = std::make_shared<PublicAccount>("BBVA", 23.45);
    auto cat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto transaction = std::make_shared<PublicTransaction>(account, 23.0, cat);
    auto recurrence = std::make_shared<PublicRecurrence>(
            com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate());
    auto recurrent = std::make_shared<PublicRecurrentTransaction>(transaction, recurrence);

    PublicBook book;
    book.store(cat);
    QVERIFY(!book.isError());

    book.store(recurrent);
    QVERIFY(book.isError());
}

void
TestBookRecurrentTransaction::testStoreCategoryNotStored() {
    auto account = std::make_shared<PublicAccount>("BBVA", 23.45);
    auto cat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto transaction = std::make_shared<PublicTransaction>(account, 23.0, cat);
    auto recurrence = std::make_shared<PublicRecurrence>(
            com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate());
    auto recurrent = std::make_shared<PublicRecurrentTransaction>(transaction, recurrence);

    PublicBook book;
    book.store(account);
    QVERIFY(!book.isError());

    book.store(recurrent);
    QVERIFY(book.isError());
}

void
TestBookRecurrentTransaction::testStoreLastGeneratedPresent_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<QDate>("lastGenerated");
    QTest::addColumn<chancho::RecurrentTransaction::Recurrence::Defaults>("defaults");
    QTest::addColumn<PublicTransactionPtr>("transaction");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto firstLastGenerated = QDate(2014, 3, 1);
    auto firstDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::DAILY;
    auto firstTran = std::make_shared<PublicTransaction>(firstAcc, 23.9, firstCat);

    QTest::newRow("first-obj") << firstAcc << firstCat << firstLastGenerated  << firstDefaults << firstTran;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondLastGenerated = QDate(2017, 12, 1);
    auto secondDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY;
    auto secondTran = std::make_shared<PublicTransaction>(secondAcc, 90.0, secondCat);

    QTest::newRow("second-obj") << secondAcc << secondCat << secondLastGenerated << secondDefaults << secondTran;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    auto lastLastGenerated = QDate(2017, 1, 17);
    auto lastDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY;
    auto lastTran = std::make_shared<PublicTransaction>(lastAcc, 8.0, lastCat);

    QTest::newRow("last-obj") << lastAcc << lastCat << lastLastGenerated << lastDefaults << lastTran;
}

void
TestBookRecurrentTransaction::testStoreLastGeneratedPresent() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(QDate, lastGenerated);
    QFETCH(chancho::RecurrentTransaction::Recurrence::Defaults , defaults);
    QFETCH(PublicTransactionPtr, transaction);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto currentDate = QDate::currentDate();
    auto recurrence = std::make_shared<PublicRecurrence>(defaults, currentDate);
    recurrence->lastGenerated = lastGenerated;
    auto recurrent = std::make_shared<PublicRecurrentTransaction>(transaction, recurrence);

    book.store(recurrent);
    QVERIFY(recurrent->wasStoredInDb());

    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", recurrent->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    if (transaction->category->type == com::chancho::Category::Type::EXPENSE && transaction->amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount * -1));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount));
    }
    QCOMPARE(query->value("account").toString(), account->_dbId.toString());
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("contents").toString(), transaction->contents);
    QCOMPARE(query->value("memo").toString(), transaction->memo);
    QCOMPARE(query->value("startDay").toInt(), currentDate.day());
    QCOMPARE(query->value("startMonth").toInt(), currentDate.month());
    QCOMPARE(query->value("startYear").toInt(), currentDate.year());
    QCOMPARE(query->value("lastDay").toInt(), lastGenerated.day());
    QCOMPARE(query->value("lastMonth").toInt(), lastGenerated.month());
    QCOMPARE(query->value("lastYear").toInt(), lastGenerated.year());
    QVERIFY(query->value("endDay").isNull());
    QVERIFY(query->value("endMonth").isNull());
    QVERIFY(query->value("endYear").isNull());
    QCOMPARE(query->value("defaultType").toInt(), static_cast<int>(defaults));
    QVERIFY(query->value("numberDays").isNull());
    QVERIFY(query->value("occurrences").isNull());

    db->close();
}

void
TestBookRecurrentTransaction::testStoreLasteGeneratedMissing_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<chancho::RecurrentTransaction::Recurrence::Defaults>("defaults");
    QTest::addColumn<PublicTransactionPtr>("transaction");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto firstDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::DAILY;
    auto firstTran = std::make_shared<PublicTransaction>(firstAcc, 23.9, firstCat);

    QTest::newRow("first-obj") << firstAcc << firstCat << firstDefaults << firstTran;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY;
    auto secondTran = std::make_shared<PublicTransaction>(secondAcc, 90.0, secondCat);

    QTest::newRow("second-obj") << secondAcc << secondCat << secondDefaults << secondTran;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    auto lastDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY;
    auto lastTran = std::make_shared<PublicTransaction>(lastAcc, 8.0, lastCat);

    QTest::newRow("last-obj") << lastAcc << lastCat << lastDefaults << lastTran;
}

void
TestBookRecurrentTransaction::testStoreLasteGeneratedMissing() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(chancho::RecurrentTransaction::Recurrence::Defaults , defaults);
    QFETCH(PublicTransactionPtr, transaction);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto currentDate = QDate::currentDate();
    auto recurrence = std::make_shared<PublicRecurrence>(defaults, currentDate);
    auto recurrent = std::make_shared<PublicRecurrentTransaction>(transaction, recurrence);

    book.store(recurrent);
    QVERIFY(recurrent->wasStoredInDb());

    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", recurrent->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    if (transaction->category->type == com::chancho::Category::Type::EXPENSE && transaction->amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount * -1));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount));
    }
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("contents").toString(), transaction->contents);
    QCOMPARE(query->value("memo").toString(), transaction->memo);
    QCOMPARE(query->value("startDay").toInt(), currentDate.day());
    QCOMPARE(query->value("startMonth").toInt(), currentDate.month());
    QCOMPARE(query->value("startYear").toInt(), currentDate.year());
    QVERIFY(query->value("lastDay").isNull());
    QVERIFY(query->value("lastMonth").isNull());
    QVERIFY(query->value("lastYear").isNull());
    QVERIFY(query->value("endDay").isNull());
    QVERIFY(query->value("endMonth").isNull());
    QVERIFY(query->value("endYear").isNull());
    QCOMPARE(query->value("defaultType").toInt(), static_cast<int>(defaults));
    QVERIFY(query->value("numberDays").isNull());
    QVERIFY(query->value("occurrences").isNull());

    db->close();
}

void
TestBookRecurrentTransaction::testStoreEndDatePresent_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<QDate>("endDate");
    QTest::addColumn<chancho::RecurrentTransaction::Recurrence::Defaults>("defaults");
    QTest::addColumn<PublicTransactionPtr>("transaction");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    QDate firstEndDate(2015, 2, 2);
    auto firstDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::DAILY;
    auto firstTran = std::make_shared<PublicTransaction>(firstAcc, 23.9, firstCat);

    QTest::newRow("first-obj") << firstAcc << firstCat << firstEndDate << firstDefaults << firstTran;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    QDate secondEndDate(2020, 4, 5);
    auto secondDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY;
    auto secondTran = std::make_shared<PublicTransaction>(secondAcc, 90.0, secondCat);

    QTest::newRow("second-obj") << secondAcc << secondCat << secondEndDate << secondDefaults << secondTran;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    QDate lastEndDate(2030, 1, 2);
    auto lastDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY;
    auto lastTran = std::make_shared<PublicTransaction>(lastAcc, 8.0, lastCat);

    QTest::newRow("last-obj") << lastAcc << lastCat << lastEndDate << lastDefaults << lastTran;
}

void
TestBookRecurrentTransaction::testStoreEndDatePresent() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(QDate, endDate);
    QFETCH(chancho::RecurrentTransaction::Recurrence::Defaults , defaults);
    QFETCH(PublicTransactionPtr, transaction);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto currentDate = QDate::currentDate();
    auto recurrence = std::make_shared<PublicRecurrence>(defaults, currentDate, endDate);
    auto recurrent = std::make_shared<PublicRecurrentTransaction>(transaction, recurrence);

    book.store(recurrent);
    QVERIFY(recurrent->wasStoredInDb());

    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", recurrent->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    if (transaction->category->type == com::chancho::Category::Type::EXPENSE && transaction->amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount * -1));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount));
    }
    QCOMPARE(query->value("account").toString(), account->_dbId.toString());
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("contents").toString(), transaction->contents);
    QCOMPARE(query->value("memo").toString(), transaction->memo);
    QCOMPARE(query->value("startDay").toInt(), currentDate.day());
    QCOMPARE(query->value("startMonth").toInt(), currentDate.month());
    QCOMPARE(query->value("startYear").toInt(), currentDate.year());
    QVERIFY(query->value("lastDay").isNull());
    QVERIFY(query->value("lastMonth").isNull());
    QVERIFY(query->value("lastYear").isNull());
    QCOMPARE(query->value("endDay").toInt(), endDate.day());
    QCOMPARE(query->value("endMonth").toInt(), endDate.month());
    QCOMPARE(query->value("endYear").toInt(), endDate.year());
    QCOMPARE(query->value("defaultType").toInt(), static_cast<int>(defaults));
    QVERIFY(query->value("numberDays").isNull());
    QVERIFY(query->value("occurrences").isNull());

    db->close();
}

void
TestBookRecurrentTransaction::testStoreEndDateMissing_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<chancho::RecurrentTransaction::Recurrence::Defaults>("defaults");
    QTest::addColumn<PublicTransactionPtr>("transaction");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto firstDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::DAILY;
    auto firstTran = std::make_shared<PublicTransaction>(firstAcc, 23.9, firstCat);

    QTest::newRow("first-obj") << firstAcc << firstCat << firstDefaults << firstTran;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY;
    auto secondTran = std::make_shared<PublicTransaction>(secondAcc, 90.0, secondCat);

    QTest::newRow("second-obj") << secondAcc << secondCat << secondDefaults << secondTran;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    auto lastDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY;
    auto lastTran = std::make_shared<PublicTransaction>(lastAcc, 8.0, lastCat);

    QTest::newRow("last-obj") << lastAcc << lastCat << lastDefaults << lastTran;
}

void
TestBookRecurrentTransaction::testStoreEndDateMissing() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(chancho::RecurrentTransaction::Recurrence::Defaults , defaults);
    QFETCH(PublicTransactionPtr, transaction);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto currentDate = QDate::currentDate();
    auto recurrence = std::make_shared<PublicRecurrence>(defaults, currentDate);
    auto recurrent = std::make_shared<PublicRecurrentTransaction>(transaction, recurrence);

    book.store(recurrent);
    QVERIFY(recurrent->wasStoredInDb());

    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", recurrent->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    if (transaction->category->type == com::chancho::Category::Type::EXPENSE && transaction->amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount * -1));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount));
    }
    QCOMPARE(query->value("account").toString(), account->_dbId.toString());
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("contents").toString(), transaction->contents);
    QCOMPARE(query->value("memo").toString(), transaction->memo);
    QCOMPARE(query->value("startDay").toInt(), currentDate.day());
    QCOMPARE(query->value("startMonth").toInt(), currentDate.month());
    QCOMPARE(query->value("startYear").toInt(), currentDate.year());
    QVERIFY(query->value("lastDay").isNull());
    QVERIFY(query->value("lastMonth").isNull());
    QVERIFY(query->value("lastYear").isNull());
    QVERIFY(query->value("endDay").isNull());
    QVERIFY(query->value("endMonth").isNull());
    QVERIFY(query->value("endYear").isNull());
    QCOMPARE(query->value("defaultType").toInt(), static_cast<int>(defaults));
    QVERIFY(query->value("numberDays").isNull());
    QVERIFY(query->value("occurrences").isNull());

    db->close();
}

void
TestBookRecurrentTransaction::testStoreDefaultsUsed_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<chancho::RecurrentTransaction::Recurrence::Defaults>("defaults");
    QTest::addColumn<PublicTransactionPtr>("transaction");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto firstDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::DAILY;
    auto firstTran = std::make_shared<PublicTransaction>(firstAcc, 23.9, firstCat);

    QTest::newRow("first-obj") << firstAcc << firstCat << firstDefaults << firstTran;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY;
    auto secondTran = std::make_shared<PublicTransaction>(secondAcc, 90.0, secondCat);

    QTest::newRow("second-obj") << secondAcc << secondCat << secondDefaults << secondTran;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    auto lastDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY;
    auto lastTran = std::make_shared<PublicTransaction>(lastAcc, 8.0, lastCat);

    QTest::newRow("last-obj") << lastAcc << lastCat << lastDefaults << lastTran;
}

void
TestBookRecurrentTransaction::testStoreDefaultsUsed() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(chancho::RecurrentTransaction::Recurrence::Defaults , defaults);
    QFETCH(PublicTransactionPtr, transaction);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto currentDate = QDate::currentDate();
    auto recurrence = std::make_shared<PublicRecurrence>(defaults, currentDate);
    auto recurrent = std::make_shared<PublicRecurrentTransaction>(transaction, recurrence);

    book.store(recurrent);
    QVERIFY(recurrent->wasStoredInDb());

    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", recurrent->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    if (transaction->category->type == com::chancho::Category::Type::EXPENSE && transaction->amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount * -1));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount));
    }
    QCOMPARE(query->value("account").toString(), account->_dbId.toString());
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("contents").toString(), transaction->contents);
    QCOMPARE(query->value("memo").toString(), transaction->memo);
    QCOMPARE(query->value("startDay").toInt(), currentDate.day());
    QCOMPARE(query->value("startMonth").toInt(), currentDate.month());
    QCOMPARE(query->value("startYear").toInt(), currentDate.year());
    QVERIFY(query->value("lastDay").isNull());
    QVERIFY(query->value("lastMonth").isNull());
    QVERIFY(query->value("lastYear").isNull());
    QVERIFY(query->value("endDay").isNull());
    QVERIFY(query->value("endMonth").isNull());
    QVERIFY(query->value("endYear").isNull());
    QCOMPARE(query->value("defaultType").toInt(), static_cast<int>(defaults));
    QVERIFY(query->value("numberDays").isNull());
    QVERIFY(query->value("occurrences").isNull());

    db->close();
}

void
TestBookRecurrentTransaction::testStoreNonDefaultsUsed_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<int>("days");
    QTest::addColumn<PublicTransactionPtr>("transaction");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto firstTran = std::make_shared<PublicTransaction>(firstAcc, 23.9, firstCat);

    QTest::newRow("first-obj") << firstAcc << firstCat << 8 << firstTran;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondTran = std::make_shared<PublicTransaction>(secondAcc, 90.0, secondCat);

    QTest::newRow("second-obj") << secondAcc << secondCat << 5 << secondTran;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    auto lastTran = std::make_shared<PublicTransaction>(lastAcc, 8.0, lastCat);

    QTest::newRow("last-obj") << lastAcc << lastCat << 22 << lastTran;
}

void
TestBookRecurrentTransaction::testStoreNonDefaultsUsed() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(int, days);
    QFETCH(PublicTransactionPtr, transaction);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto currentDate = QDate::currentDate();
    auto recurrence = std::make_shared<PublicRecurrence>(days, currentDate);
    auto recurrent = std::make_shared<PublicRecurrentTransaction>(transaction, recurrence);

    book.store(recurrent);
    QVERIFY(recurrent->wasStoredInDb());

    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", recurrent->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    if (transaction->category->type == com::chancho::Category::Type::EXPENSE && transaction->amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount * -1));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount));
    }
    QCOMPARE(query->value("account").toString(), account->_dbId.toString());
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("contents").toString(), transaction->contents);
    QCOMPARE(query->value("memo").toString(), transaction->memo);
    QCOMPARE(query->value("startDay").toInt(), currentDate.day());
    QCOMPARE(query->value("startMonth").toInt(), currentDate.month());
    QCOMPARE(query->value("startYear").toInt(), currentDate.year());
    QVERIFY(query->value("lastDay").isNull());
    QVERIFY(query->value("lastMonth").isNull());
    QVERIFY(query->value("lastYear").isNull());
    QVERIFY(query->value("endDay").isNull());
    QVERIFY(query->value("endMonth").isNull());
    QVERIFY(query->value("endYear").isNull());
    QVERIFY(query->value("defaultType").isNull());
    QCOMPARE(query->value("numberDays").toInt(), days);
    QVERIFY(query->value("occurrences").isNull());

    db->close();
}

void
TestBookRecurrentTransaction::testStoreOcurrencesPresent_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<chancho::RecurrentTransaction::Recurrence::Defaults>("defaults");
    QTest::addColumn<PublicTransactionPtr>("transaction");
    QTest::addColumn<int>("ocurrences");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto firstDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::DAILY;
    auto firstTran = std::make_shared<PublicTransaction>(firstAcc, 23.9, firstCat);

    QTest::newRow("first-obj") << firstAcc << firstCat << firstDefaults << firstTran << 5;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY;
    auto secondTran = std::make_shared<PublicTransaction>(secondAcc, 90.0, secondCat);

    QTest::newRow("second-obj") << secondAcc << secondCat << secondDefaults << secondTran << 10;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    auto lastDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY;
    auto lastTran = std::make_shared<PublicTransaction>(lastAcc, 8.0, lastCat);

    QTest::newRow("last-obj") << lastAcc << lastCat << lastDefaults << lastTran << 20;
}

void
TestBookRecurrentTransaction::testStoreOcurrencesPresent() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(chancho::RecurrentTransaction::Recurrence::Defaults , defaults);
    QFETCH(PublicTransactionPtr, transaction);
    QFETCH(int, ocurrences);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto currentDate = QDate::currentDate();
    auto recurrence = std::make_shared<PublicRecurrence>(defaults, currentDate);
    recurrence->occurrences = ocurrences;
    auto recurrent = std::make_shared<PublicRecurrentTransaction>(transaction, recurrence);

    book.store(recurrent);
    QVERIFY(recurrent->wasStoredInDb());

    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", recurrent->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    if (transaction->category->type == com::chancho::Category::Type::EXPENSE && transaction->amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount * -1));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount));
    }
    QCOMPARE(query->value("account").toString(), account->_dbId.toString());
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("contents").toString(), transaction->contents);
    QCOMPARE(query->value("memo").toString(), transaction->memo);
    QCOMPARE(query->value("startDay").toInt(), currentDate.day());
    QCOMPARE(query->value("startMonth").toInt(), currentDate.month());
    QCOMPARE(query->value("startYear").toInt(), currentDate.year());
    QVERIFY(query->value("lastDay").isNull());
    QVERIFY(query->value("lastMonth").isNull());
    QVERIFY(query->value("lastYear").isNull());
    QVERIFY(query->value("endDay").isNull());
    QVERIFY(query->value("endMonth").isNull());
    QVERIFY(query->value("endYear").isNull());
    QCOMPARE(query->value("defaultType").toInt(), static_cast<int>(defaults));
    QVERIFY(query->value("numberDays").isNull());
    QCOMPARE(query->value("occurrences").toInt(), ocurrences);

    db->close();
}

void
TestBookRecurrentTransaction::testStoreOcurrencesMissing_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<chancho::RecurrentTransaction::Recurrence::Defaults>("defaults");
    QTest::addColumn<PublicTransactionPtr>("transaction");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto firstDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::DAILY;
    auto firstTran = std::make_shared<PublicTransaction>(firstAcc, 23.9, firstCat);

    QTest::newRow("first-obj") << firstAcc << firstCat << firstDefaults << firstTran;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY;
    auto secondTran = std::make_shared<PublicTransaction>(secondAcc, 90.0, secondCat);

    QTest::newRow("second-obj") << secondAcc << secondCat << secondDefaults << secondTran;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    auto lastDefaults = chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY;
    auto lastTran = std::make_shared<PublicTransaction>(lastAcc, 8.0, lastCat);

    QTest::newRow("last-obj") << lastAcc << lastCat << lastDefaults << lastTran;
}

void
TestBookRecurrentTransaction::testStoreOcurrencesMissing() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(chancho::RecurrentTransaction::Recurrence::Defaults , defaults);
    QFETCH(PublicTransactionPtr, transaction);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto currentDate = QDate::currentDate();
    auto recurrence = std::make_shared<PublicRecurrence>(defaults, currentDate);
    auto recurrent = std::make_shared<PublicRecurrentTransaction>(transaction, recurrence);

    book.store(recurrent);
    QVERIFY(recurrent->wasStoredInDb());

    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", recurrent->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    if (transaction->category->type == com::chancho::Category::Type::EXPENSE && transaction->amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount * -1));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(transaction->amount));
    }
    QCOMPARE(query->value("account").toString(), account->_dbId.toString());
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("contents").toString(), transaction->contents);
    QCOMPARE(query->value("memo").toString(), transaction->memo);
    QCOMPARE(query->value("startDay").toInt(), currentDate.day());
    QCOMPARE(query->value("startMonth").toInt(), currentDate.month());
    QCOMPARE(query->value("startYear").toInt(), currentDate.year());
    QVERIFY(query->value("lastDay").isNull());
    QVERIFY(query->value("lastMonth").isNull());
    QVERIFY(query->value("lastYear").isNull());
    QVERIFY(query->value("endDay").isNull());
    QVERIFY(query->value("endMonth").isNull());
    QVERIFY(query->value("endYear").isNull());
    QCOMPARE(query->value("defaultType").toInt(), static_cast<int>(defaults));
    QVERIFY(query->value("numberDays").isNull());
    QVERIFY(query->value("occurrences").isNull());

    db->close();
}

void
TestBookRecurrentTransaction::testRecurrentTransactions_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<QList<chancho::RecurrentTransactionPtr>>("transacions");
    auto acc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto cat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);

    QList<chancho::RecurrentTransactionPtr> firstList;
    firstList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 30, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));

    QTest::newRow("single-result") << acc << cat << firstList;

    QList<chancho::RecurrentTransactionPtr> secondList;
    secondList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 89, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY, QDate::currentDate())
    ));
    secondList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 9, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, QDate::currentDate())
    ));
    secondList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 19, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));

    QTest::newRow("multiple-results") << acc << cat << secondList;

    QList<chancho::RecurrentTransactionPtr> lastList;

    QTest::newRow("empty-results") << acc << cat << lastList;
}

void
TestBookRecurrentTransaction::testRecurrentTransactions() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(QList<chancho::RecurrentTransactionPtr>, transacions);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    if (category->wasStoredInDb()) {
        auto public_ptr = std::static_pointer_cast<PublicCategory>(category);
        public_ptr->_dbId = QUuid();
    }

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    book.store(transacions);
    QVERIFY(!book.isError());

    auto result = book.recurrentTransactions();
    QVERIFY(!book.isError());
    QCOMPARE(result.count(), transacions.count());
}

void
TestBookRecurrentTransaction::testRecurrentTransactionsLimit_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<QList<chancho::RecurrentTransactionPtr>>("transacions");
    auto acc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto cat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);

    QList<chancho::RecurrentTransactionPtr> firstList;
    firstList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 30, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));

    QTest::newRow("single-result") << acc << cat << firstList;

    QList<chancho::RecurrentTransactionPtr> secondList;
    secondList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 89, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY, QDate::currentDate())
    ));
    secondList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 9, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, QDate::currentDate())
    ));
    secondList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 19, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));

    QTest::newRow("multiple-results") << acc << cat << secondList;
}

void
TestBookRecurrentTransaction::testRecurrentTransactionsLimit() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(QList<chancho::RecurrentTransactionPtr>, transacions);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    if (category->wasStoredInDb()) {
        auto public_ptr = std::static_pointer_cast<PublicCategory>(category);
        public_ptr->_dbId = QUuid();
    }

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    book.store(transacions);
    QVERIFY(!book.isError());

    auto result = book.recurrentTransactions(1);
    QVERIFY(!book.isError());
    QCOMPARE(result.count(), 1);
}

void
TestBookRecurrentTransaction::testRecurrentTransactionsOffset_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<int>("limit");
    QTest::addColumn<int>("offset");
    QTest::addColumn<QList<chancho::RecurrentTransactionPtr>>("transacions");
    auto acc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto cat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);

    QList<chancho::RecurrentTransactionPtr> firstList;
    firstList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 30, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));
    firstList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 30, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));

    QTest::newRow("two-result") << acc << cat << 1 << 1 << firstList;

    QList<chancho::RecurrentTransactionPtr> secondList;
    secondList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 89, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY, QDate::currentDate())
    ));
    secondList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 9, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, QDate::currentDate())
    ));
    secondList.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 19, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));

    QTest::newRow("multiple-results") << acc << cat << 1 << 2 << secondList;

}

void
TestBookRecurrentTransaction::testRecurrentTransactionsOffset() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(int, limit);
    QFETCH(int, offset);
    QFETCH(QList<chancho::RecurrentTransactionPtr>, transacions);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    if (category->wasStoredInDb()) {
        auto public_ptr = std::static_pointer_cast<PublicCategory>(category);
        public_ptr->_dbId = QUuid();
    }

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    book.store(transacions);
    QVERIFY(!book.isError());

    auto result = book.recurrentTransactions(limit, offset);
    QVERIFY(!book.isError());
    QCOMPARE(result.count(), limit);
}

void
TestBookRecurrentTransaction::testRemoveStoredTransaction() {
    auto acc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto cat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);

    auto trans  = std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 30, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate()));

    PublicBook book;
    book.store(acc);
    QVERIFY(acc->wasStoredInDb());

    book.store(cat);
    QVERIFY(cat->wasStoredInDb());

    book.store(trans);
    QVERIFY(trans->wasStoredInDb());
    auto id = trans->_dbId;

    book.remove(trans);
    QVERIFY(!book.isError());

    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", id);
    auto success = query->exec();
    QVERIFY(success);
    QVERIFY(!query->next());
}

void
TestBookRecurrentTransaction::testRemoveMissingTransaction() {
    auto acc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto cat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);

    auto trans  = std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 30, cat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate()));

    PublicBook book;
    book.remove(trans);
    QVERIFY(book.isError());
}

void
TestBookRecurrentTransaction::testGenerateRecurrentTransactions() {
    // create a list of recurrent transactions wit diff last dates and state that all the required transactions are
    // generated and the date of the last generate one has been update
    auto acc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto cat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto currentDate = QDate::currentDate();
    auto startDate = currentDate.addMonths(-1);

    QList<chancho::RecurrentTransactionPtr> trans;
    trans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 19, cat, startDate),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, startDate)
    ));
    trans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 21, cat, currentDate.addMonths(-1)),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, startDate)
    ));
    trans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 150, cat, currentDate.addMonths(-1)),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY, startDate)
    ));
    trans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(acc, 150, cat, startDate),
            std::make_shared<PublicRecurrence>(3, currentDate.addMonths(-1))
    ));

    PublicBook book;
    book.store(acc);
    QVERIFY(!book.isError());

    book.store(cat);
    QVERIFY(!book.isError());

    book.store(trans);
    QVERIFY(!book.isError());

    // generate the transactions and ensure that the data is present
    book.generateRecurrentTransactions();

    auto count = 0;
    foreach(const chancho::RecurrentTransactionPtr& recurrent, trans){
        auto public_ptr = std::static_pointer_cast<PublicRecurrence>(recurrent->recurrence);
        count += public_ptr->generateMissingDates().count();
    }

    // we expect to have 46 transactions
    auto transactions = book.transactions(startDate.month(), startDate.year());
    transactions.append(book.transactions(currentDate.month(), currentDate.year()));
    QCOMPARE(transactions.count(), count);
}

void
TestBookRecurrentTransaction::testNumberOfRecurrentTransactions_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<QList<chancho::RecurrentTransactionPtr>>("transactions");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);

    QList<chancho::RecurrentTransactionPtr> firstTrans;
    firstTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(firstAcc, 30, firstCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));

    QTest::newRow("first-obj") << firstAcc << firstCat << firstTrans;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    QList<chancho::RecurrentTransactionPtr> secondTrans;
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 89, secondCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY, QDate::currentDate())
    ));
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 9, secondCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, QDate::currentDate())
    ));
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 19, secondCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));

    QTest::newRow("second-obj") << secondAcc << secondCat << secondTrans;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    QList<chancho::RecurrentTransactionPtr> lastTrans;

    QTest::newRow("last-obj") << lastAcc << lastCat << lastTrans;
}

void
TestBookRecurrentTransaction::testNumberOfRecurrentTransactions() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(QList<chancho::RecurrentTransactionPtr>, transactions);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    book.store(transactions);
    QVERIFY(!book.isError());

    auto count = book.numberOfRecurrentTransactions();
    QCOMPARE(count, transactions.count());
}

void
TestBookRecurrentTransaction::testRecurrentTransactionsForCategory_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<QList<PublicCategoryPtr>>("categories");
    QTest::addColumn<QPair<PublicCategoryPtr, int>>("categoryCountPair");
    QTest::addColumn<QList<chancho::RecurrentTransactionPtr>>("transactions");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    QList<PublicCategoryPtr> firstCategories;
    firstCategories.append(firstCat);

    QList<chancho::RecurrentTransactionPtr> firstTrans;
    firstTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(firstAcc, 30, firstCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));

    QTest::newRow("first-obj") << firstAcc << firstCategories << QPair<PublicCategoryPtr, int>(firstCat, 1)
        << firstTrans;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondFirstCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondSecondCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    QList<PublicCategoryPtr> secondCategories;
    secondCategories.append(secondFirstCat);
    secondCategories.append(secondSecondCat);

    QList<chancho::RecurrentTransactionPtr> secondTrans;
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 89, secondFirstCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY, QDate::currentDate())
    ));
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 9, secondFirstCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, QDate::currentDate())
    ));
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 19, secondFirstCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 19, secondSecondCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 9, secondSecondCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, QDate::currentDate())
    ));

    QTest::newRow("second-obj") << secondAcc << secondCategories << QPair<PublicCategoryPtr, int>(secondFirstCat, 3)
        << secondTrans;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    QList<PublicCategoryPtr> thirdCategories;
    secondCategories.append(lastCat);
    QList<chancho::RecurrentTransactionPtr> lastTrans;

    QTest::newRow("last-obj") << lastAcc << thirdCategories << QPair<PublicCategoryPtr, int>(lastCat, 0) << lastTrans;
}

void
TestBookRecurrentTransaction::testRecurrentTransactionsForCategory() {
    typedef QPair<PublicCategoryPtr, int> CatPair;
    QFETCH(PublicAccountPtr, account);
    QFETCH(QList<PublicCategoryPtr>, categories);
    QFETCH(CatPair, categoryCountPair);
    QFETCH(QList<chancho::RecurrentTransactionPtr>, transactions);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    foreach(const PublicCategoryPtr& cat, categories) {
        book.store(cat);
        QVERIFY(cat->wasStoredInDb());
    }

    book.store(transactions);
    QVERIFY(!book.isError());

    auto trans = book.recurrentTransactions(categoryCountPair.first);
    QCOMPARE(trans.count(), categoryCountPair.second);
}

void
TestBookRecurrentTransaction::testNumberOfRecurrentTransactionsCategory_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<QList<QPair<PublicCategoryPtr, int>>>("categoryCountPair");
    QTest::addColumn<QList<chancho::RecurrentTransactionPtr>>("transactions");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.4);
    auto firstCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    QList<QPair<PublicCategoryPtr, int>> firstCategories;
    firstCategories.append(QPair<PublicCategoryPtr, int>(firstCat, 1));

    QList<chancho::RecurrentTransactionPtr> firstTrans;
    firstTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(firstAcc, 30, firstCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));

    QTest::newRow("first-obj") << firstAcc <<  firstCategories << firstTrans;

    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 30);
    auto secondFirstCat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondSecondCat = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    QList<QPair<PublicCategoryPtr, int>> secondCategories;
    secondCategories.append(QPair<PublicCategoryPtr, int>(secondFirstCat, 3));
    secondCategories.append(QPair<PublicCategoryPtr, int>(secondSecondCat, 2));

    QList<chancho::RecurrentTransactionPtr> secondTrans;
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 89, secondFirstCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY, QDate::currentDate())
    ));
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 9, secondFirstCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, QDate::currentDate())
    ));
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 19, secondFirstCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 19, secondSecondCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate())
    ));
    secondTrans.append(std::make_shared<PublicRecurrentTransaction>(
            std::make_shared<PublicTransaction>(secondAcc, 9, secondSecondCat),
            std::make_shared<PublicRecurrence>(
                    chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, QDate::currentDate())
    ));

    QTest::newRow("second-obj") << secondAcc << secondCategories << secondTrans;

    auto lastAcc = std::make_shared<PublicAccount>("ING", 0);
    auto lastCat = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME);
    QList<QPair<PublicCategoryPtr, int>> thirdCategories;
    thirdCategories.append(QPair<PublicCategoryPtr, int>(lastCat, 0));
    QList<chancho::RecurrentTransactionPtr> lastTrans;

    QTest::newRow("last-obj") << lastAcc << thirdCategories << lastTrans;
}

void
TestBookRecurrentTransaction::testNumberOfRecurrentTransactionsCategory() {
    typedef QPair<PublicCategoryPtr, int> CatPair;
    typedef QList<CatPair> CatPairList;
    QFETCH(PublicAccountPtr, account);
    QFETCH(CatPairList, categoryCountPair);
    QFETCH(QList<chancho::RecurrentTransactionPtr>, transactions);

    PublicBook book;
    book.store(account);
    QVERIFY(account->wasStoredInDb());

    foreach(const CatPair& pair, categoryCountPair) {
        book.store(pair.first);
        QVERIFY(pair.first->wasStoredInDb());
    }

    book.store(transactions);
    QVERIFY(!book.isError());

    foreach(const CatPair& pair, categoryCountPair) {
        auto count = book.numberOfRecurrentTransactions(pair.first);
        QCOMPARE(count, pair.second);
    }
}

QTEST_MAIN(TestBookRecurrentTransaction)

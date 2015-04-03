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

#include <com/chancho/system/database.h>
#include <com/chancho/system/database_factory.h>

#include "public_account.h"
#include "public_category.h"
#include "test_book_transaction.h"

namespace sys = com::chancho::system;

namespace {
    const QString SELECT_TRANSACTION_QUERY =
            "SELECT amount, account, category, day, month, year, contents, memo FROM Transactions WHERE uuid=:uuid";
    const QString SELECT_ACCOUNT_QUERY = "SELECT name, amount, memo FROM Accounts WHERE uuid=:uuid";
}

void
TestBookTransaction::init() {
    BaseTestCase::init();
    PublicBook::initDatabse();
}

void
TestBookTransaction::cleanup() {
    BaseTestCase::cleanup();

    auto dbPath = PublicBook::databasePath();
    QFileInfo fi(dbPath);
    if (fi.exists())
        removeDir(fi.absolutePath());
}

void
TestBookTransaction::testStoreTransaction_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<double>("amount");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<QDate>("date");
    QTest::addColumn<QString>("contents");
    QTest::addColumn<QString>("memo");

    auto account = std::make_shared<PublicAccount>("BBVA", 23.45);
    auto incomeCategory = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto expenseCategory = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);

    QTest::newRow("income-obj") << account << 2000.23 << incomeCategory
            << QDate::fromString("1999-10-15", "yyyy-MM-dd") << "Canonical salary" << "August";
    QTest::newRow("expense-obj") << account << 200.0 << expenseCategory
            << QDate::fromString("2000-10-15", "yyyy-MM-dd") << "Streetxo dinner" << "";
}

void
TestBookTransaction::testStoreTransaction() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(double, amount);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(QDate, date);
    QFETCH(QString, contents);
    QFETCH(QString, memo);

    PublicBook book;

    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto transaction = std::make_shared<PublicTransaction>(account, amount, category, date, contents, memo);
    book.store(transaction);

    QVERIFY(transaction->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", transaction->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("account").toString(), account->_dbId.toString());
    if (transaction->type() == chancho::Category::Type::EXPENSE && amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(-1 * amount));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(amount));
    }
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("day").toInt(), date.day());
    QCOMPARE(query->value("month").toInt(), date.month());
    QCOMPARE(query->value("year").toInt(), date.year());
    QCOMPARE(query->value("contents").toString(), contents);
    QCOMPARE(query->value("memo").toString(), memo);

    // do assert that the account was updated
    query->prepare(SELECT_ACCOUNT_QUERY);
    query->bindValue(":uuid", account->_dbId.toString());
    success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), account->name);
    if (category->type == chancho::Category::Type::EXPENSE) {
        QCOMPARE(query->value("amount").toString(), QString::number(account->amount - transaction->amount));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(account->amount + transaction->amount));
    }
    QCOMPARE(query->value("memo").toString(), account->memo);

    db->close();
}

void
TestBookTransaction::testUpdateTransaction_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<double>("amount");
    QTest::addColumn<double>("newAmount");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<QDate>("date");
    QTest::addColumn<QString>("contents");
    QTest::addColumn<QString>("memo");

    auto account = std::make_shared<PublicAccount>("BBVA", 23.45);
    auto incomeCategory = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto expenseCategory = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);

    QTest::newRow("income-obj") << account << 2000.23 << 2.23 << incomeCategory
            << QDate::fromString("1999-10-15", "yyyy-MM-dd") << "Canonical salary" << "August";
    QTest::newRow("expense-obj") << account << 200.0 << 100.0 << expenseCategory
            << QDate::fromString("2000-10-15", "yyyy-MM-dd") << "Streetxo dinner" << "";
}

void
TestBookTransaction::testUpdateTransaction() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(double, amount);
    QFETCH(double, newAmount);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(QDate, date);
    QFETCH(QString, contents);
    QFETCH(QString, memo);

    PublicBook book;

    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto transaction = std::make_shared<PublicTransaction>(account, amount, category, date, contents, memo);
    book.store(transaction);

    // update the amount of the transaction and store it again
    transaction->amount = newAmount;
    book.store(transaction);

    QVERIFY(transaction->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", transaction->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("account").toString(), account->_dbId.toString());
    if (transaction->type() == chancho::Category::Type::EXPENSE && amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(-1 * newAmount));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(newAmount));
    }
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("day").toInt(), date.day());
    QCOMPARE(query->value("month").toInt(), date.month());
    QCOMPARE(query->value("year").toInt(), date.year());
    QCOMPARE(query->value("contents").toString(), contents);
    QCOMPARE(query->value("memo").toString(), memo);

    // do assert that the account was updated
    query->prepare(SELECT_ACCOUNT_QUERY);
    query->bindValue(":uuid", account->_dbId.toString());
    success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), account->name);
    if (category->type == chancho::Category::Type::EXPENSE) {
        QCOMPARE(query->value("amount").toString(), QString::number(account->amount - newAmount));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(account->amount + newAmount));
    }
    QCOMPARE(query->value("memo").toString(), account->memo);

    db->close();
}

void
TestBookTransaction::testStoreTransactionNoAccount() {
    auto acc = std::make_shared<chancho::Account>("Bankia", 89.2, "My savings");
    auto cat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto transaction = std::make_shared<PublicTransaction>(acc, 87.2, cat, QDate::currentDate(), QString(), QString());
    PublicBook book;
    // do not store the acc
    book.store(cat);
    book.store(transaction);

    QVERIFY(book.isError());
}

void
TestBookTransaction::testStoreTransactionNoCategory() {
    auto acc = std::make_shared<chancho::Account>("Bankia", 89.2, "My savings");
    auto cat = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto transaction = std::make_shared<PublicTransaction>(acc, 87.2, cat, QDate::currentDate(), QString(), QString());
    PublicBook book;
    book.store(acc);
    // do not store the cat
    book.store(transaction);

    QVERIFY(book.isError());
}

void
TestBookTransaction::testRemoveTransaction_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<double>("amount");
    QTest::addColumn<PublicCategoryPtr>("category");
    QTest::addColumn<QDate>("date");
    QTest::addColumn<QString>("contents");
    QTest::addColumn<QString>("memo");

    auto account = std::make_shared<PublicAccount>("BBVA", 23.45);
    auto incomeCategory = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto expenseCategory = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);

    QTest::newRow("income-obj") << account << 2000.23 << incomeCategory
            << QDate::fromString("1999-10-15", "yyyy-MM-dd") << "Canonical salary" << "August";
    QTest::newRow("expense-obj") << account << 200.0 << expenseCategory
            << QDate::fromString("2000-10-15", "yyyy-MM-dd") << "Streetxo dinner" << "";
}

void
TestBookTransaction::testRemoveTransaction() {
    QFETCH(PublicAccountPtr, account);
    QFETCH(double, amount);
    QFETCH(PublicCategoryPtr, category);
    QFETCH(QDate, date);
    QFETCH(QString, contents);
    QFETCH(QString, memo);

    PublicBook book;

    book.store(account);
    QVERIFY(account->wasStoredInDb());

    book.store(category);
    QVERIFY(category->wasStoredInDb());

    auto transaction = std::make_shared<PublicTransaction>(account, amount, category, date, contents, memo);
    book.store(transaction);

    QVERIFY(transaction->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_TRANSACTION_QUERY);
    query->bindValue(":uuid", transaction->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("account").toString(), account->_dbId.toString());
    if (transaction->type() == chancho::Category::Type::EXPENSE && amount > 0) {
        QCOMPARE(query->value("amount").toString(), QString::number(-1 * amount));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(amount));
    }
    QCOMPARE(query->value("category").toString(), category->_dbId.toString());
    QCOMPARE(query->value("day").toInt(), date.day());
    QCOMPARE(query->value("month").toInt(), date.month());
    QCOMPARE(query->value("year").toInt(), date.year());
    QCOMPARE(query->value("contents").toString(), contents);
    QCOMPARE(query->value("memo").toString(), memo);

    // do assert that the account was updated
    query->prepare(SELECT_ACCOUNT_QUERY);
    query->bindValue(":uuid", account->_dbId.toString());
    success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), account->name);
    if (category->type == chancho::Category::Type::EXPENSE) {
        QCOMPARE(query->value("amount").toString(), QString::number(account->amount - transaction->amount));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(account->amount + transaction->amount));
    }
    QCOMPARE(query->value("memo").toString(), account->memo);

    db->close();

    // delete the transaction and assert that the amount of the account is the expected one, that is with the
    // transaction being removed
    book.remove(transaction);
    QVERIFY(!transaction->wasStoredInDb());

    opened = db->open();
    QVERIFY(opened);

    query = db->createQuery();
    query->prepare(SELECT_ACCOUNT_QUERY);
    query->bindValue(":uuid", account->_dbId.toString());
    success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), account->name);
    if (category->type == chancho::Category::Type::EXPENSE) {
        QCOMPARE(query->value("amount").toString(), QString::number(account->amount));
    } else {
        QCOMPARE(query->value("amount").toString(), QString::number(account->amount));
    }
    QCOMPARE(query->value("memo").toString(), account->memo);

    db->close();
}

void
TestBookTransaction::testTransactionsMonth_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<std::shared_ptr<PublicCategory>>("category");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto category = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 23.0, category, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 25.6, category, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 90.9, category, QDate(2015, 2, 2), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 40.2, category, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 10.5, category, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 90.0, category, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 32.0, category, QDate(2014, 1, 13), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 89.4, category, QDate(2014, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 90.2, category, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 290., category, QDate(2014, 8, 11), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);

    QTest::newRow("feb-2015") << account << category << 2 << 2015 << firstExpected << all;
    QTest::newRow("jan-2014") << account << category << 1 << 2014 << secondExpected << all;
    QTest::newRow("august-2014") << account << category << 8 << 2014 << thirdExpected << all;
}

void
TestBookTransaction::testTransactionsMonth() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(std::shared_ptr<PublicCategory>, category);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    if (category->wasStoredInDb()) {
        category->_dbId = QUuid();
    }
    book.store(category);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    // we have stored all books, we perform the query and then assert the results using the expected list
    auto transactions = book.transactions(month, year);
    QCOMPARE(transactions.count(), expected.count());

    QStringList expectedList;
    foreach(const chancho::TransactionPtr tran, expected) {
        expectedList.append(tran->contents);
    }
    foreach(const chancho::TransactionPtr tran, transactions) {
        QVERIFY(expectedList.contains(tran->contents));
    }
}

void
TestBookTransaction::testTransactionsMonthCount_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto category = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 23.0, category, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 25.6, category, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 90.9, category, QDate(2015, 2, 2), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 40.2, category, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 10.5, category, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 90.0, category, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 32.0, category, QDate(2014, 1, 13), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 89.4, category, QDate(2014, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 90.2, category, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 290., category, QDate(2014, 8, 11), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);

    QTest::newRow("feb-2015") << account << category << 2 << 2015 << firstExpected << all;
    QTest::newRow("jan-2014") << account << category << 1 << 2014 << secondExpected << all;
    QTest::newRow("august-2014") << account << category << 8 << 2014 << thirdExpected << all;
}

void
TestBookTransaction::testTransactionsMonthCount() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    book.store(category);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    auto count = book.numberOfTransactions(month, year);
    QCOMPARE(count, expected.count());
}

void
TestBookTransaction::testTransactionsMonthLimited_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<std::shared_ptr<PublicCategory>>("category");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto category = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 23.0, category, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 25.6, category, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 90.9, category, QDate(2015, 2, 2), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 40.2, category, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 10.5, category, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 90.0, category, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 32.0, category, QDate(2014, 1, 13), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 89.4, category, QDate(2014, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 90.2, category, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 290., category, QDate(2014, 8, 11), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);

    QTest::newRow("feb-2015") << account << category << 2 << 2015 << firstExpected << all;
    QTest::newRow("jan-2014") << account << category << 1 << 2014 << secondExpected << all;
    QTest::newRow("august-2014") << account << category << 8 << 2014 << thirdExpected << all;
}

void
TestBookTransaction::testTransactionsMonthLimited() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(std::shared_ptr<PublicCategory>, category);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    if (category->wasStoredInDb()) {
        category->_dbId = QUuid();
    }
    book.store(category);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    auto result = book.transactions(month, year, 1, 0);
    QVERIFY(!book.isError());
    QCOMPARE(1, result.count());
}

void
TestBookTransaction::testTransactionsDay_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<std::shared_ptr<PublicCategory>>("category");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto category = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 23.0, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 25.6, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 90.9, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 40.2, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 10.5, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 90.0, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 32.0, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 89.4, category, QDate(2015, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 90.2, category, QDate(2015, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 290., category, QDate(2015, 8, 20), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);

    QTest::newRow("feb-2015") << account << category << 1 << 2 << 2015 << firstExpected << all;
    QTest::newRow("jan-2014") << account << category << 30 << 1 << 2015 << secondExpected << all;
    QTest::newRow("august-2014") << account << category << 20 << 8 << 2015 << thirdExpected << all;
}

void
TestBookTransaction::testTransactionsDay() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(std::shared_ptr<PublicCategory>, category);
    QFETCH(int, day);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    if (category->wasStoredInDb()) {
        category->_dbId = QUuid();
    }
    book.store(category);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    // we have stored all books, we perform the query and then assert the results using the expected list
    auto transactions = book.transactions(day, month, year);
    QCOMPARE(transactions.count(), expected.count());

    QStringList expectedList;
    foreach(const chancho::TransactionPtr tran, expected) {
        expectedList.append(tran->contents);
    }
    foreach(const chancho::TransactionPtr tran, transactions) {
        QVERIFY(expectedList.contains(tran->contents));
    }
}

void
TestBookTransaction::testTransactionsDayCount_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto category = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 23.0, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 25.6, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 90.9, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 40.2, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 10.5, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 90.0, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 32.0, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 89.4, category, QDate(2015, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 90.2, category, QDate(2015, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 290., category, QDate(2015, 8, 20), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);

    QTest::newRow("feb-2015") << account << category << 1 << 2 << 2015 << firstExpected << all;
    QTest::newRow("jan-2014") << account << category << 30 << 1 << 2015 << secondExpected << all;
    QTest::newRow("august-2014") << account << category << 20 << 8 << 2015 << thirdExpected << all;
}

void
TestBookTransaction::testTransactionsDayCount() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(int, day);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    book.store(category);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    auto count = book.numberOfTransactions(day, month, year);
    QCOMPARE(count, expected.count());
}

void
TestBookTransaction::testTransactionsDayLimited_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<std::shared_ptr<PublicCategory>>("category");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto category = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 23.0, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 25.6, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 90.9, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 40.2, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 10.5, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 90.0, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 32.0, category, QDate(2015, 1, 30), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 89.4, category, QDate(2015, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 90.2, category, QDate(2015, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 290., category, QDate(2015, 8, 20), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);

    QTest::newRow("feb-2015") << account << category << 1 << 2 << 2015 << firstExpected << all;
    QTest::newRow("jan-2014") << account << category << 30 << 1 << 2015 << secondExpected << all;
    QTest::newRow("august-2014") << account << category << 20 << 8 << 2015 << thirdExpected << all;
}

void
TestBookTransaction::testTransactionsDayLimited() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(std::shared_ptr<PublicCategory>, category);
    QFETCH(int, day);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    if (category->wasStoredInDb()) {
        category->_dbId = QUuid();
    }
    book.store(category);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    auto result = book.transactions(day, month, year, 1, 0);
    QVERIFY(!book.isError());
    QCOMPARE(1, result.count());
}

void
TestBookTransaction::testGetMonthsWithTransactions_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<int>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("transactions");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto category = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);

    QList<std::shared_ptr<PublicTransaction>> firstTrans;
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 23.0, category, QDate(2015, 4, 3), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 25.6, category, QDate(2015, 4, 6), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 90.9, category, QDate(2015, 6, 2), QUuid::createUuid().toString()));

    QList<int> firstMonths;
    firstMonths.append(2);
    firstMonths.append(4);
    firstMonths.append(6);

    QList<std::shared_ptr<PublicTransaction>> secondTrans;
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 40.2, category, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 10.5, category, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 90.0, category, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 32.0, category, QDate(2014, 1, 13), QUuid::createUuid().toString()));
    QList<int> secondMonths;
    secondMonths.append(1);

    QList<std::shared_ptr<PublicTransaction>> thirdTrans;
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 89.4, category, QDate(2014, 8, 20), QUuid::createUuid().toString()));
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 90.2, category, QDate(2014, 9, 12), QUuid::createUuid().toString()));
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 290., category, QDate(2014, 9, 11), QUuid::createUuid().toString()));
    QList<int> thirdMonths;
    thirdMonths.append(8);
    thirdMonths.append(9);

    QTest::newRow("feb-2015") << account << category << 2015 << firstMonths << firstTrans;
    QTest::newRow("jan-2014") << account << category << 2014 << secondMonths << secondTrans;
    QTest::newRow("august-2014") << account << category << 2014 << thirdMonths << thirdTrans;
}

void
TestBookTransaction::testGetMonthsWithTransactions() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;
    typedef QList<int> IntList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(int, year);
    QFETCH(IntList, expected);
    QFETCH(TransactionList, transactions);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    book.store(category);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicTransaction> tran, transactions) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    auto result = book.monthsWithTransactions(year);
    QVERIFY(!book.isError());
    QCOMPARE(expected.count(), result.count());
    int lastMonth = -1;
    foreach(int month, result) {
        QVERIFY(expected.contains(month));
        if (lastMonth == -1) {
            lastMonth = month;
        } else {
            QVERIFY(lastMonth > month);
            lastMonth = month;
        }
    }
}

void
TestBookTransaction::testGetMonthsWithTransactionsCount_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<int>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("transactions");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto category = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);

    QList<std::shared_ptr<PublicTransaction>> firstTrans;
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 23.0, category, QDate(2015, 4, 3), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 25.6, category, QDate(2015, 4, 6), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 90.9, category, QDate(2015, 6, 2), QUuid::createUuid().toString()));

    QList<int> firstMonths;
    firstMonths.append(2);
    firstMonths.append(4);
    firstMonths.append(6);

    QList<std::shared_ptr<PublicTransaction>> secondTrans;
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 40.2, category, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 10.5, category, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 90.0, category, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 32.0, category, QDate(2014, 1, 13), QUuid::createUuid().toString()));
    QList<int> secondMonths;
    secondMonths.append(1);

    QList<std::shared_ptr<PublicTransaction>> thirdTrans;
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 89.4, category, QDate(2014, 8, 20), QUuid::createUuid().toString()));
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 90.2, category, QDate(2014, 9, 12), QUuid::createUuid().toString()));
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 290., category, QDate(2014, 9, 11), QUuid::createUuid().toString()));
    QList<int> thirdMonths;
    thirdMonths.append(8);
    thirdMonths.append(9);

    QTest::newRow("feb-2015") << account << category << 2015 << firstMonths << firstTrans;
    QTest::newRow("jan-2014") << account << category << 2014 << secondMonths << secondTrans;
    QTest::newRow("august-2014") << account << category << 2014 << thirdMonths << thirdTrans;
}

void
TestBookTransaction::testGetMonthsWithTransactionsCount() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;
    typedef QList<int> IntList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(int, year);
    QFETCH(IntList, expected);
    QFETCH(TransactionList, transactions);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    book.store(category);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicTransaction> tran, transactions) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    auto result = book.numberOfMonthsWithTransactions(year);
    QVERIFY(!book.isError());
    QCOMPARE(expected.count(), result);
}

void
TestBookTransaction::testGetDaysWithTransactions_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<int>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("transactions");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto category = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);

    QList<std::shared_ptr<PublicTransaction>> firstTrans;
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 23.0, category, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 25.6, category, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 90.9, category, QDate(2015, 2, 2), QUuid::createUuid().toString()));

    QList<int> firstMonths;
    firstMonths.append(1);
    firstMonths.append(2);
    firstMonths.append(3);

    QList<std::shared_ptr<PublicTransaction>> secondTrans;
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 40.2, category, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 10.5, category, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 90.0, category, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 32.0, category, QDate(2014, 1, 13), QUuid::createUuid().toString()));
    QList<int> secondMonths;
    secondMonths.append(30);
    secondMonths.append(15);
    secondMonths.append(28);
    secondMonths.append(13);

    QList<std::shared_ptr<PublicTransaction>> thirdTrans;
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 89.4, category, QDate(2014, 9, 20), QUuid::createUuid().toString()));
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 90.2, category, QDate(2014, 9, 12), QUuid::createUuid().toString()));
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 290., category, QDate(2014, 9, 11), QUuid::createUuid().toString()));
    QList<int> thirdMonths;
    thirdMonths.append(20);
    thirdMonths.append(12);
    thirdMonths.append(11);

    QTest::newRow("feb-2015") << account << category << 2 << 2015 << firstMonths << firstTrans;
    QTest::newRow("jan-2014") << account << category << 1 << 2014 << secondMonths << secondTrans;
    QTest::newRow("august-2014") << account << category << 9 << 2014 << thirdMonths << thirdTrans;
}

void
TestBookTransaction::testGetDaysWithTransactions() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;
    typedef QList<int> IntList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(IntList, expected);
    QFETCH(TransactionList, transactions);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    book.store(category);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicTransaction> tran, transactions) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    auto result = book.daysWithTransactions(month, year);
    QVERIFY(!book.isError());
    QCOMPARE(expected.count(), result.count());
    int lastDay = -1;
    foreach(int day, result) {
        QVERIFY(expected.contains(day));
        if (lastDay == -1) {
            lastDay = day;
        } else {
            QVERIFY(lastDay > day);
            lastDay = day;
        }
    }
}

void
TestBookTransaction::testGetDaysWithTransactionsCount_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<int>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("transactions");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto category = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);

    QList<std::shared_ptr<PublicTransaction>> firstTrans;
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 23.0, category, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 25.6, category, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstTrans.append(std::make_shared<PublicTransaction>(
            account, 90.9, category, QDate(2015, 2, 2), QUuid::createUuid().toString()));

    QList<int> firstMonths;
    firstMonths.append(1);
    firstMonths.append(2);
    firstMonths.append(3);

    QList<std::shared_ptr<PublicTransaction>> secondTrans;
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 40.2, category, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 10.5, category, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 90.0, category, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondTrans.append(std::make_shared<PublicTransaction>(
            account, 32.0, category, QDate(2014, 1, 13), QUuid::createUuid().toString()));
    QList<int> secondMonths;
    secondMonths.append(30);
    secondMonths.append(15);
    secondMonths.append(28);
    secondMonths.append(13);

    QList<std::shared_ptr<PublicTransaction>> thirdTrans;
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 89.4, category, QDate(2014, 9, 20), QUuid::createUuid().toString()));
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 90.2, category, QDate(2014, 9, 12), QUuid::createUuid().toString()));
    thirdTrans.append(std::make_shared<PublicTransaction>(
            account, 290., category, QDate(2014, 9, 11), QUuid::createUuid().toString()));
    QList<int> thirdMonths;
    thirdMonths.append(20);
    thirdMonths.append(12);
    thirdMonths.append(11);

    QTest::newRow("feb-2015") << account << category << 2 << 2015 << firstMonths << firstTrans;
    QTest::newRow("jan-2014") << account << category << 1 << 2014 << secondMonths << secondTrans;
    QTest::newRow("august-2014") << account << category << 9 << 2014 << thirdMonths << thirdTrans;
}

void
TestBookTransaction::testGetDaysWithTransactionsCount() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;
    typedef QList<int> IntList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(IntList, expected);
    QFETCH(TransactionList, transactions);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    book.store(category);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicTransaction> tran, transactions) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    auto result = book.numberOfDaysWithTransactions(month, year);
    QVERIFY(!book.isError());
    QCOMPARE(expected.count(), result);
}

void
TestBookTransaction::testTransactionsCategory_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<std::shared_ptr<PublicCategory>>("category");
    QTest::addColumn<QList<std::shared_ptr<PublicCategory>>>("categories");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto firstCategory = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondCategory = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto thirdCategory = std::make_shared<PublicCategory>("Cinema", chancho::Category::Type::EXPENSE);
    QList<std::shared_ptr<PublicCategory>> categories;
    categories.append(firstCategory);
    categories.append(secondCategory);
    categories.append(thirdCategory);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 100.2, firstCategory, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 23.0, firstCategory, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 25.6, firstCategory, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 90.9, firstCategory, QDate(2015, 2, 2), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 40.2, secondCategory, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 10.5, secondCategory, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 90.0, secondCategory, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 32.0, secondCategory, QDate(2014, 1, 13), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 89.4, thirdCategory, QDate(2014, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 90.2, thirdCategory, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 290., thirdCategory, QDate(2014, 8, 11), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);

    QTest::newRow("feb-2015") << account << firstCategory << categories << firstExpected << all;
    QTest::newRow("jan-2014") << account << secondCategory << categories << secondExpected << all;
    QTest::newRow("august-2014") << account << thirdCategory << categories << thirdExpected << all;
}

void
TestBookTransaction::testTransactionsCategory() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;
    typedef QList<std::shared_ptr<PublicCategory>> CategoriesList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(std::shared_ptr<PublicCategory>, category);
    QFETCH(CategoriesList, categories);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicCategory>& cat, categories) {
        if(cat->wasStoredInDb()) {
            cat->_dbId = QUuid();
        }
        book.store(cat);
        QVERIFY(!book.isError());
        QVERIFY(cat->wasStoredInDb());
    }

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    // we have stored all books, we perform the query and then assert the results using the expected list
    auto transactions = book.transactions(category);
    QCOMPARE(transactions.count(), expected.count());

    QStringList expectedList;
    foreach(const chancho::TransactionPtr tran, expected) {
        expectedList.append(tran->contents);
    }
    foreach(const chancho::TransactionPtr tran, transactions) {
        QVERIFY(expectedList.contains(tran->contents));
    }
}

void
TestBookTransaction::testTransactionsCategoryAndMonth_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<std::shared_ptr<PublicCategory>>("category");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<std::shared_ptr<PublicCategory>>>("categories");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto firstCategory = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondCategory = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto thirdCategory = std::make_shared<PublicCategory>("Cinema", chancho::Category::Type::EXPENSE);
    QList<std::shared_ptr<PublicCategory>> categories;
    categories.append(firstCategory);
    categories.append(secondCategory);
    categories.append(thirdCategory);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 100.2, firstCategory, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 23.0, firstCategory, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 25.6, firstCategory, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 90.9, firstCategory, QDate(2015, 2, 2), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 40.2, secondCategory, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 10.5, secondCategory, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 90.0, secondCategory, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 32.0, secondCategory, QDate(2014, 1, 13), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 89.4, thirdCategory, QDate(2014, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 90.2, thirdCategory, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 290., thirdCategory, QDate(2014, 8, 11), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);
    all.append(std::make_shared<PublicTransaction>(
            account, 100.2, firstCategory, QDate(2014, 2, 1), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 23.0, firstCategory, QDate(2015, 1, 3), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 25.6, firstCategory, QDate(2017, 2, 6), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 90.9, firstCategory, QDate(2015, 10, 2), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 40.2, secondCategory, QDate(2014, 2, 30), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 10.5, secondCategory, QDate(2015, 2, 15), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 90.0, secondCategory, QDate(2014, 8, 28), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 32.0, secondCategory, QDate(2014, 3, 13), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 89.4, thirdCategory, QDate(2013, 8, 20), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 90.2, thirdCategory, QDate(2015, 8, 12), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 290., thirdCategory, QDate(2014, 9, 11), QUuid::createUuid().toString()));

    QTest::newRow("feb-2015") << account << firstCategory << 2 << 2015 << categories << firstExpected << all;
    QTest::newRow("jan-2014") << account << secondCategory << 1 << 2014 << categories << secondExpected << all;
    QTest::newRow("august-2014") << account << thirdCategory << 8 << 2014 << categories << thirdExpected << all;
}

void
TestBookTransaction::testTransactionsCategoryAndMonth() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;
    typedef QList<std::shared_ptr<PublicCategory>> CategoriesList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(std::shared_ptr<PublicCategory>, category);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(CategoriesList, categories);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicCategory>& cat, categories) {
        if (cat->wasStoredInDb()) {
            cat->_dbId = QUuid();
        }
        book.store(cat);
        QVERIFY(!book.isError());
        QVERIFY(cat->wasStoredInDb());
    }

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    // we have stored all books, we perform the query and then assert the results using the expected list
    auto transactions = book.transactions(category, month, year);
    QCOMPARE(transactions.count(), expected.count());

    QStringList expectedList;
    foreach(const chancho::TransactionPtr tran, expected) {
        expectedList.append(tran->contents);
    }
    foreach(const chancho::TransactionPtr tran, transactions) {
        QVERIFY(expectedList.contains(tran->contents));
    }
}

void
TestBookTransaction::testTransactionsAccount_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<QList<chancho::AccountPtr>>("accounts");
    QTest::addColumn<std::shared_ptr<PublicCategory>>("category");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto firstAccount = std::make_shared<chancho::Account>("Bankia", 0);
    auto secondAccount = std::make_shared<chancho::Account>("BBVA", 0);
    auto thirdAccount = std::make_shared<chancho::Account>("ING", 0);
    auto category = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);

    QList<chancho::AccountPtr> accounts;
    accounts.append(firstAccount);
    accounts.append(secondAccount);
    accounts.append(thirdAccount);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            firstAccount, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            firstAccount, 23.0, category, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            firstAccount, 25.6, category, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            firstAccount, 90.9, category, QDate(2015, 2, 2), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            secondAccount, 40.2, category, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            secondAccount, 10.5, category, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            secondAccount, 90.0, category, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            secondAccount, 32.0, category, QDate(2014, 1, 13), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            thirdAccount, 89.4, category, QDate(2014, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            thirdAccount, 90.2, category, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            thirdAccount, 290., category, QDate(2014, 8, 11), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);

    QTest::newRow("feb-2015") << firstAccount << accounts << category << firstExpected << all;
    QTest::newRow("jan-2014") << secondAccount << accounts << category << secondExpected << all;
    QTest::newRow("august-2014") << thirdAccount << accounts << category << thirdExpected << all;
}

void
TestBookTransaction::testTransactionsAccount() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;
    typedef QList<chancho::AccountPtr> AccountsList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(AccountsList, accounts);
    QFETCH(std::shared_ptr<PublicCategory>, category);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    foreach(const chancho::AccountPtr acc, accounts) {
        book.store(acc);
        QVERIFY(acc->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    if (category->wasStoredInDb()) {
        category->_dbId = QUuid();
    }
    book.store(category);
    QVERIFY(!book.isError());
    QVERIFY(category->wasStoredInDb());

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    // we have stored all books, we perform the query and then assert the results using the expected list
    auto transactions = book.transactions(account);
    QCOMPARE(transactions.count(), expected.count());

    QStringList expectedList;
    foreach(const chancho::TransactionPtr tran, expected) {
        expectedList.append(tran->contents);
    }
    foreach(const chancho::TransactionPtr tran, transactions) {
        QVERIFY(expectedList.contains(tran->contents));
    }
}

void
TestBookTransaction::testDeleteAccountTransactions_data() {
    QTest::addColumn<PublicAccountPtr>("account");
    QTest::addColumn<QList<PublicAccountPtr>>("accounts");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto firstAccount = std::make_shared<PublicAccount>("Bankia", 0);
    auto secondAccount = std::make_shared<PublicAccount>("BBVA", 0);
    auto thirdAccount = std::make_shared<PublicAccount>("ING", 0);
    auto category = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);

    QList<PublicAccountPtr> accounts;
    accounts.append(firstAccount);
    accounts.append(secondAccount);
    accounts.append(thirdAccount);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            firstAccount, 100.2, category, QDate(2015, 2, 1), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            firstAccount, 23.0, category, QDate(2015, 2, 3), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            firstAccount, 25.6, category, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            firstAccount, 90.9, category, QDate(2015, 2, 2), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            secondAccount, 40.2, category, QDate(2014, 1, 30), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            secondAccount, 10.5, category, QDate(2014, 1, 15), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            secondAccount, 90.0, category, QDate(2014, 1, 28), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            secondAccount, 32.0, category, QDate(2014, 1, 13), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            thirdAccount, 89.4, category, QDate(2014, 8, 20), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            thirdAccount, 90.2, category, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            thirdAccount, 290., category, QDate(2014, 8, 11), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);

    QTest::newRow("feb-2015") << firstAccount << accounts << category << all;
    QTest::newRow("jan-2014") << secondAccount << accounts << category << all;
    QTest::newRow("august-2014") << thirdAccount << accounts << category << all;
}

void
TestBookTransaction::testDeleteAccountTransactions() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;
    typedef QList<PublicAccountPtr> AccountsList;

    QFETCH(PublicAccountPtr, account);
    QFETCH(AccountsList, accounts);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(TransactionList, all);

    PublicBook book;

    foreach(const chancho::AccountPtr acc, accounts) {
        book.store(acc);
        QVERIFY(acc->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    auto oldId = account->_dbId;

    book.store(category);
    QVERIFY(!book.isError());
    QVERIFY(category->wasStoredInDb());

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    book.remove(account);
    account->_dbId = oldId;

    // we have stored all books, we perform the query and then assert the results using the expected list
    auto transactions = book.transactions(account);
    QCOMPARE(transactions.count(), 0);
}

void
TestBookTransaction::testIncomeForDay_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<std::shared_ptr<PublicCategory>>("category");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<std::shared_ptr<PublicCategory>>>("categories");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto firstCategory = std::make_shared<PublicCategory>("Food", chancho::Category::Type::INCOME);
    auto secondCategory = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);
    auto thirdCategory = std::make_shared<PublicCategory>("Cinema", chancho::Category::Type::INCOME);
    QList<std::shared_ptr<PublicCategory>> categories;
    categories.append(firstCategory);
    categories.append(secondCategory);
    categories.append(thirdCategory);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 100.2, firstCategory, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 23.0, firstCategory, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 25.6, firstCategory, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 90.9, firstCategory, QDate(2015, 2, 6), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 40.2, secondCategory, QDate(2014, 1, 14), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 10.5, secondCategory, QDate(2014, 1, 14), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 90.0, secondCategory, QDate(2014, 1, 14), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 32.0, secondCategory, QDate(2014, 1, 14), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 89.4, thirdCategory, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 90.2, thirdCategory, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 290., thirdCategory, QDate(2014, 8, 12), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);
    all.append(std::make_shared<PublicTransaction>(
            account, 100.2, firstCategory, QDate(2014, 2, 1), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 23.0, firstCategory, QDate(2015, 1, 3), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 25.6, firstCategory, QDate(2017, 2, 6), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 90.9, firstCategory, QDate(2015, 10, 2), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 40.2, secondCategory, QDate(2014, 2, 30), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 10.5, secondCategory, QDate(2015, 2, 15), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 90.0, secondCategory, QDate(2014, 8, 28), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 32.0, secondCategory, QDate(2014, 3, 13), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 89.4, thirdCategory, QDate(2013, 8, 20), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 90.2, thirdCategory, QDate(2015, 8, 12), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 290., thirdCategory, QDate(2014, 9, 11), QUuid::createUuid().toString()));

    QTest::newRow("feb-2015") << account << firstCategory << 6 << 2 << 2015 << categories << firstExpected << all;
    QTest::newRow("jan-2014") << account << secondCategory << 14 << 1 << 2014 << categories << secondExpected << all;
    QTest::newRow("august-2014") << account << thirdCategory << 12 << 8 << 2014 << categories << thirdExpected << all;
}

void
TestBookTransaction::testIncomeForDay() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;
    typedef QList<std::shared_ptr<PublicCategory>> CategoriesList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(std::shared_ptr<PublicCategory>, category);
    QFETCH(int, day);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(CategoriesList, categories);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());

    foreach(const std::shared_ptr<PublicCategory>& cat, categories) {
        if (cat->wasStoredInDb()) {
            cat->_dbId = QUuid();
        }
        book.store(cat);
        QVERIFY(!book.isError());
        QVERIFY(cat->wasStoredInDb());
    }
    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    double expectedAmount = 0;
    foreach(const std::shared_ptr<PublicTransaction> tran, expected) {
        expectedAmount += tran->amount;
    }

    // we have stored all books, we perform the query and then assert the results using the expected list
    auto result = book.incomeForDay(day, month, year);
    QCOMPARE(result, expectedAmount);
}

void
TestBookTransaction::testExpenseForDay_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<std::shared_ptr<PublicCategory>>>("categories");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("expected");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("all");

    auto account = std::make_shared<chancho::Account>("Bankia", 0);
    auto firstCategory = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto secondCategory = std::make_shared<PublicCategory>("Salad", chancho::Category::Type::EXPENSE);
    auto thirdCategory = std::make_shared<PublicCategory>("Cinema", chancho::Category::Type::EXPENSE);
    QList<std::shared_ptr<PublicCategory>> categories;
    categories.append(firstCategory);
    categories.append(secondCategory);
    categories.append(thirdCategory);

    QList<std::shared_ptr<PublicTransaction>> firstExpected;
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 100.2, firstCategory, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 23.0, firstCategory, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 25.6, firstCategory, QDate(2015, 2, 6), QUuid::createUuid().toString()));
    firstExpected.append(std::make_shared<PublicTransaction>(
            account, 90.9, firstCategory, QDate(2015, 2, 6), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> secondExpected;
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 40.2, secondCategory, QDate(2014, 1, 14), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 10.5, secondCategory, QDate(2014, 1, 14), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 90.0, secondCategory, QDate(2014, 1, 14), QUuid::createUuid().toString()));
    secondExpected.append(std::make_shared<PublicTransaction>(
            account, 32.0, secondCategory, QDate(2014, 1, 14), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> thirdExpected;
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 89.4, thirdCategory, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 90.2, thirdCategory, QDate(2014, 8, 12), QUuid::createUuid().toString()));
    thirdExpected.append(std::make_shared<PublicTransaction>(
            account, 290., thirdCategory, QDate(2014, 8, 12), QUuid::createUuid().toString()));

    QList<std::shared_ptr<PublicTransaction>> all;
    all.append(firstExpected);
    all.append(secondExpected);
    all.append(thirdExpected);
    all.append(std::make_shared<PublicTransaction>(
            account, 100.2, firstCategory, QDate(2014, 2, 1), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 23.0, firstCategory, QDate(2015, 1, 3), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 25.6, firstCategory, QDate(2017, 2, 6), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 90.9, firstCategory, QDate(2015, 10, 2), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 40.2, secondCategory, QDate(2014, 2, 30), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 10.5, secondCategory, QDate(2015, 2, 15), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 90.0, secondCategory, QDate(2014, 8, 28), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 32.0, secondCategory, QDate(2014, 3, 13), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 89.4, thirdCategory, QDate(2013, 8, 20), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 90.2, thirdCategory, QDate(2015, 8, 12), QUuid::createUuid().toString()));
    all.append(std::make_shared<PublicTransaction>(
            account, 290., thirdCategory, QDate(2014, 9, 11), QUuid::createUuid().toString()));

    QTest::newRow("feb-2015") << account << 6 << 2 << 2015 << categories << firstExpected << all;
    QTest::newRow("jan-2014") << account << 14 << 1 << 2014 << categories << secondExpected << all;
    QTest::newRow("august-2014") << account << 12 << 8 << 2014 << categories << thirdExpected << all;
}

void
TestBookTransaction::testExpenseForDay() {
    typedef QList<std::shared_ptr<PublicTransaction>> TransactionList;
    typedef QList<std::shared_ptr<PublicCategory>> CategoriesList;

    QFETCH(chancho::AccountPtr, account);
    QFETCH(int, day);
    QFETCH(int, month);
    QFETCH(int, year);
    QFETCH(CategoriesList, categories);
    QFETCH(TransactionList, expected);
    QFETCH(TransactionList, all);

    PublicBook book;

    book.store(account);
    QVERIFY(!book.isError());
    QCOMPARE(book.numberOfAccounts(), 1);

    foreach(const std::shared_ptr<PublicCategory>& cat, categories) {
        if(cat->wasStoredInDb()) {
            cat->_dbId = QUuid();
        }
        book.store(cat);
        QVERIFY(!book.isError());
        QVERIFY(cat->wasStoredInDb());
    }
    QCOMPARE(book.numberOfCategories(), categories.count());

    foreach(const std::shared_ptr<PublicTransaction> tran, all) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(!book.isError());
        QVERIFY(tran->wasStoredInDb());
    }

    QCOMPARE(book.numberOfTransactions(), all.count());

    double expectedAmount = 0;
    foreach(const std::shared_ptr<PublicTransaction> tran, expected) {
        expectedAmount += tran->amount;
    }

    // we have stored all books, we perform the query and then assert the results using the expected list
    auto result = book.expenseForDay(day, month, year);
    QCOMPARE(result, -1 * expectedAmount);
}

void
TestBookTransaction::testMoveExpenseAccounts() {
    PublicBook book;

    auto firstAcc = std::make_shared<PublicAccount>("BBVA", 0);
    auto secondAcc = std::make_shared<PublicAccount>("Bankia", 0);

    book.store(firstAcc);
    QVERIFY(!book.isError());

    book.store(secondAcc);
    QVERIFY(!book.isError());

    auto category = std::make_shared<chancho::Category>("Test category", chancho::Category::Type::EXPENSE);
    book.store(category);
    QVERIFY(!book.isError());

    // create a transaction, store it and then update it and ensure that the amounts are correct
    auto tran = std::make_shared<PublicTransaction>(firstAcc, 30, category);
    book.store(tran);
    QVERIFY(!book.isError());

    // update the account and then set it in the db
    tran->account = secondAcc;
    book.store(tran);
    QVERIFY(!book.isError());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_ACCOUNT_QUERY);
    query->bindValue(":uuid", firstAcc->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    // assert that the first acc in set to 0
    QCOMPARE(query->value("amount").toString(), QString::number(0));

    query->bindValue(":uuid", secondAcc->_dbId.toString());
    success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    // assert that the second acc in set to the transaction value
    QCOMPARE(query->value("amount").toString(), QString::number(-1 * tran->amount));

    db->close();
}

void
TestBookTransaction::testMoveIncomeAccounts() {
    PublicBook book;

    auto firstAcc = std::make_shared<PublicAccount>("BBVA", 0);
    auto secondAcc = std::make_shared<PublicAccount>("Bankia", 0);

    book.store(firstAcc);
    QVERIFY(!book.isError());

    book.store(secondAcc);
    QVERIFY(!book.isError());

    auto category = std::make_shared<chancho::Category>("Test category", chancho::Category::Type::INCOME);
    book.store(category);
    QVERIFY(!book.isError());

    // create a transaction, store it and then update it and ensure that the amounts are correct
    auto tran = std::make_shared<PublicTransaction>(firstAcc, 30, category);
    book.store(tran);
    QVERIFY(!book.isError());

    // update the account and then set it in the db
    tran->account = secondAcc;
    book.store(tran);
    QVERIFY(!book.isError());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_ACCOUNT_QUERY);
    query->bindValue(":uuid", firstAcc->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    // assert that the first acc in set to 0
    QCOMPARE(query->value("amount").toString(), QString::number(0));

    query->bindValue(":uuid", secondAcc->_dbId.toString());
    success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    // assert that the second acc in set to the transaction value
    QCOMPARE(query->value("amount").toString(), QString::number(tran->amount));

    db->close();
}

void
TestBookTransaction::testCategoryTypeChanged() {
    PublicBook book;

    auto firstAcc = std::make_shared<PublicAccount>("BBVA", 0);

    book.store(firstAcc);
    QVERIFY(!book.isError());

    auto category = std::make_shared<chancho::Category>("Test category", chancho::Category::Type::INCOME);
    book.store(category);
    QVERIFY(!book.isError());

    // create a transaction, store it and then update it and ensure that the amounts are correct
    auto tran = std::make_shared<PublicTransaction>(firstAcc, 30, category);
    book.store(tran);
    QVERIFY(!book.isError());

    // update the category and then set it in the db
    category->type = chancho::Category::Type::EXPENSE;
    book.store(category);
    QVERIFY(!book.isError());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    auto query = db->createQuery();
    query->prepare(SELECT_ACCOUNT_QUERY);
    query->bindValue(":uuid", firstAcc->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    // assert that the first acc in set to 0
    QCOMPARE(query->value("amount").toString(), QString::number(-1 * tran->amount));

    db->close();
}

QTEST_MAIN(TestBookTransaction)

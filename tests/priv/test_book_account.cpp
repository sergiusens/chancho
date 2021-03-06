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
#include "test_book_account.h"

namespace sys = com::chancho::system;

namespace {
    const QString SELECT_ACCOUNT_QUERY = "SELECT name, initialAmount, amount, color, memo FROM Accounts WHERE uuid=:uuid";
}

void
TestBookAccount::init() {
    BaseTestCase::init();
    PublicBook::initDatabse();
}

void
TestBookAccount::cleanup() {
    BaseTestCase::cleanup();

    auto dbPath = PublicBook::databasePath();
    QFileInfo fi(dbPath);
    if (fi.exists())
        removeDir(fi.absolutePath());
}

void
TestBookAccount::testStoreAccount_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<double>("initialAmount");
    QTest::addColumn<double>("amount");
    QTest::addColumn<QString>("memo");
    QTest::addColumn<QString>("color");

    QTest::newRow("bankia-obj") << "Bankia" << 50.0 << 3004.89 << "Savings account" << "#ff";
    QTest::newRow("bbva-obj") << "BBVA" << 10.9 << 23.9 << "Student loan" << "#234523";
}

void
TestBookAccount::testStoreAccount() {
    QFETCH(QString, name);
    QFETCH(double, initialAmount);
    QFETCH(double, amount);
    QFETCH(QString, memo);
    QFETCH(QString, color);

    // create the category and store it
    auto account = std::make_shared<PublicAccount>(name, amount, memo, color);
    account->initialAmount = initialAmount;
    PublicBook book;
    book.store(account);

    QVERIFY(account->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_ACCOUNT_QUERY);
    query->bindValue(":uuid", account->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), account->name);
    QCOMPARE(query->value("amount").toString(), QString::number(account->amount));
    QCOMPARE(query->value("initialAmount").toString(), QString::number(account->initialAmount));
    QCOMPARE(query->value("memo").toString(), account->memo);
    QCOMPARE(query->value("color").toString(), account->color);

    db->close();
}

void
TestBookAccount::testUpdateAccount_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("newName");
    QTest::addColumn<double>("amount");
    QTest::addColumn<QString>("memo");
    QTest::addColumn<QString>("newMemo");
    QTest::addColumn<QString>("color");
    QTest::addColumn<QString>("newColor");

    QTest::newRow("bankia-obj") << "Bankia" << "Updated Bankia" << 189.9 << "Savings account." << "Savings"
        << "#ff" << "#345678";
    QTest::newRow("bbva-obj") << "BBVA" << "Updated BBVA" << 18909.23 << "Student loan" << "Student account"
        << "#453212" << "#fff";
}

void
TestBookAccount::testUpdateAccount() {
    QFETCH(QString, name);
    QFETCH(QString, newName);
    QFETCH(double, amount);
    QFETCH(QString, memo);
    QFETCH(QString, newMemo);
    QFETCH(QString, color);
    QFETCH(QString, newColor);

    // create the category and store it
    auto account = std::make_shared<PublicAccount>(name, amount, memo, color);
    PublicBook book;
    book.store(account);

    QVERIFY(account->wasStoredInDb());

    // update the category and assert the values
    account->name = newName;
    account->memo = newMemo;
    account->color = newColor;
    book.store(account);

    QVERIFY(account->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_ACCOUNT_QUERY);
    query->bindValue(":uuid", account->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), newName);  // test against new name to be 100% sure
    QCOMPARE(query->value("amount").toString(), QString::number(account->amount));
    QCOMPARE(query->value("memo").toString(), newMemo);
    QCOMPARE(query->value("color").toString(), newColor);

    db->close();
}

void
TestBookAccount::testRemoveAccount() {
    QString name = "Bankia";
    auto amount = 9345.5;
    QString memo = "Savings account";

    auto account = std::make_shared<PublicAccount>(name, amount, memo);

    PublicBook book;
    book.store(account);

    // assert that both the parent and the category are added
    QVERIFY(account->wasStoredInDb());

    // method under test
    book.remove(account);

    QVERIFY(!account->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_ACCOUNT_QUERY);
    query->bindValue(":uuid", account->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(!query->next());

    db->close();
}

void
TestBookAccount::testRemoveNotAdded() {
    auto acc = std::make_shared<chancho::Account>("Bankia", 89.2, "My savings");
    PublicBook book;
    book.remove(acc);
    QVERIFY(book.isError());
}

void
TestBookAccount::testGetAccontsEmpty() {
    PublicBook book;
    auto accs = book.accounts();
    QCOMPARE(accs.count(), 0);
}

void
TestBookAccount::testGetAcconts() {
    QList<com::chancho::AccountPtr> accs;
    accs.append(std::make_shared<PublicAccount>("Bankia", 89.2, "Savings account"));
    accs.append(std::make_shared<PublicAccount>("BBVA", 89082.1, "Student loan"));
    accs.append(std::make_shared<PublicAccount>("HSBC", 23890, "Not declared."));

    PublicBook book;
    book.store(accs);

    QStringList ids;
    foreach(const com::chancho::AccountPtr acc, accs) {
        QVERIFY(acc->wasStoredInDb());
        ids.append(acc->name);
    }

    auto result = book.accounts();

    QCOMPARE(ids.count(), result.count());
    foreach(const chancho::AccountPtr& acc, result) {
        QVERIFY(ids.contains(acc->name));
    }
}

void
TestBookAccount::testNumberOfAccounts() {
    QList<com::chancho::AccountPtr> accs;
    accs.append(std::make_shared<PublicAccount>("Bankia", 89.2, "Savings account"));
    accs.append(std::make_shared<PublicAccount>("BBVA", 89082.1, "Student loan"));
    accs.append(std::make_shared<PublicAccount>("HSBC", 23890, "Not declared."));

    PublicBook book;
    book.store(accs);

    foreach(const com::chancho::AccountPtr acc, accs) {
        QVERIFY(acc->wasStoredInDb());
    }

    auto number = book.numberOfAccounts();
    QCOMPARE(number, 3);
}

void
TestBookAccount::testAccountsLimit() {
    QList<com::chancho::AccountPtr> accs;
    accs.append(std::make_shared<PublicAccount>("BBVA", 89.2, "Savings account"));
    accs.append(std::make_shared<PublicAccount>("Bankia", 89082.1, "Student loan"));
    accs.append(std::make_shared<PublicAccount>("HSBC", 23890, "Not declared."));

    PublicBook book;
    book.store(accs);

    foreach(const com::chancho::AccountPtr acc, accs) {
        QVERIFY(acc->wasStoredInDb());
    }

    auto firstResult = book.accounts(1, 0);
    QCOMPARE(firstResult.at(0)->name, accs.at(0)->name);

    auto secondResult = book.accounts(1, 1);
    QCOMPARE(secondResult.at(0)->name, accs.at(1)->name);

    auto lastResult = book.accounts(1, 2);
    QCOMPARE(lastResult.at(0)->name, accs.at(2)->name);
}

void
TestBookAccount::testTransactionsNotStored() {
    auto acc = std::make_shared<PublicAccount>("BBVA", 89.2, "Savings account");

    PublicBook book;
    auto result = book.transactions(acc);
    QCOMPARE(result.count(), 0);
}

QTEST_MAIN(TestBookAccount)

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
    const QString SELECT_ACCOUNT_QUERY = "SELECT name, amount, memo FROM Accounts WHERE uuid=:uuid";
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
    QTest::addColumn<double>("amount");
    QTest::addColumn<QString>("memo");

    QTest::newRow("bankia-obj") << "Bankia" << 3004.89 << "Savings account";
    QTest::newRow("bbva-obj") << "BBVA" << 23.9 << "Student loan";
}

void
TestBookAccount::testStoreAccount() {
    QFETCH(QString, name);
    QFETCH(double, amount);
    QFETCH(QString, memo);

    // create the category and store it
    auto account = std::make_shared<PublicAccount>(name, amount, memo);
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
    QCOMPARE(query->value("amount").toInt(), static_cast<int>(ceil(100.0 * account->amount)));
    QCOMPARE(query->value("memo").toString(), account->memo);

    db->close();
}

void
TestBookAccount::testUpdateAccount_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("newName");
    QTest::addColumn<double>("amount");
    QTest::addColumn<QString>("memo");
    QTest::addColumn<QString>("newMemo");

    QTest::newRow("bankia-obj") << "Bankia" << "Updated Bankia" << 189.9 << "Savings account." << "Savings";
    QTest::newRow("bbva-obj") << "BBVA" << "Updated BBVA" << 18909.23 << "Student loan" << "Student account";
}

void
TestBookAccount::testUpdateAccount() {
    QFETCH(QString, name);
    QFETCH(QString, newName);
    QFETCH(double, amount);
    QFETCH(QString, memo);
    QFETCH(QString, newMemo);

    // create the category and store it
    auto account = std::make_shared<PublicAccount>(name, amount, memo);
    PublicBook book;
    book.store(account);

    QVERIFY(account->wasStoredInDb());

    // update the category and assert the values
    account->name = newName;
    account->memo = newMemo;
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
    QCOMPARE(query->value("amount").toInt(), static_cast<int>(ceil(100.0 * account->amount)));
    QCOMPARE(query->value("memo").toString(), newMemo);

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
    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 89.2, "Savings account");
    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 89082.1, "Student loan");
    auto lastAcc = std::make_shared<PublicAccount>("HSBC", 23890, "Not declared.");

    PublicBook book;
    book.store(firstAcc);
    book.store(secondAcc);
    book.store(lastAcc);

    QVERIFY(firstAcc->wasStoredInDb());
    QVERIFY(secondAcc->wasStoredInDb());
    QVERIFY(lastAcc->wasStoredInDb());

    QStringList ids;
    ids.append(firstAcc->name);
    ids.append(secondAcc->name);
    ids.append(lastAcc->name);

    auto accs = book.accounts();

    QCOMPARE(ids.count(), accs.count());
    foreach(const chancho::AccountPtr& acc, accs) {
        QVERIFY(ids.contains(acc->name));
    }
}

QTEST_MAIN(TestBookAccount)

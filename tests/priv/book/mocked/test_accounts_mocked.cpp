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

#include <memory>

#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

#include <com/chancho/category.h>

#include "matchers.h"
#include "public_account.h"
#include "public_transaction.h"
#include "test_accounts_mocked.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;
using ::testing::Sequence;

namespace com {

namespace chancho {

namespace tests {

namespace {
    const QString INSERT_UPDATE_ACCOUNT = "INSERT OR REPLACE INTO Accounts(uuid, name, memo, color, initialAmount, amount) " \
    "VALUES (:uuid, :name, :memo, :color, :initialAmount, :amount)";
    const QString SELECT_ALL_ACCOUNTS = "SELECT uuid, name, memo, color, initialAmount, amount FROM Accounts ORDER BY name ASC";
    const QString SELECT_ALL_ACCOUNTS_LIMIT = "SELECT uuid, name, memo, color, initialAmount, amount FROM Accounts ORDER BY name ASC "\
    "LIMIT :limit OFFSET :offset";
    const QString DELETE_ACCOUNT = "DELETE FROM Accounts WHERE uuid=:uuid";
    const QString SELECT_ACCOUNTS_COUNT = "SELECT count(*) FROM Accounts";
}

void
TestAccountsMocked::init() {
    BaseTestCase::init();
    _dbFactory = new tests::MockDatabaseFactory();

    chancho::system::DatabaseFactory::setInstance(_dbFactory);
}

void
TestAccountsMocked::cleanup() {
    BaseTestCase::cleanup();
    QFileInfo fi(PublicBook::databasePath());
    if (fi.exists()) {
        QFile::remove(PublicBook::databasePath());
    }
    chancho::system::DatabaseFactory::deleteInstance();
}

void
TestAccountsMocked::testStoreAccountExecError() {
    QSqlError error(QString(QTest::currentTestFunction()), "Test Error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();
    auto acc = std::make_shared<chancho::Account>("Bankia", 53.23, "Savings");

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillOnce(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(INSERT_UPDATE_ACCOUNT)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":uuid"),
                          Matcher<const QVariant&>(_),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":name"),
                          Matcher<const QVariant&>(acc->name),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":memo"),
                          Matcher<const QVariant&>(acc->memo),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":color"),
                          Matcher<const QVariant&>(acc->color),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":initialAmount"),
                          Matcher<const QVariant&>(QString::number(acc->initialAmount)),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":amount"),
                          Matcher<const QVariant&>(QString::number(acc->amount)),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;

    book.store(acc);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testStoreAccount_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<double>("amount");
    QTest::addColumn<double>("initialAmount");
    QTest::addColumn<QString>("memo");
    QTest::addColumn<QString>("color");

    QTest::newRow("first-acc") << "Bankia" <<  90.0 << 34.0 << "My first acc" << "white";
    QTest::newRow("second-acc") << "ING" << 0.4 << 2.4 << "Savings" << "black";
    QTest::newRow("third-acc") << "BBVA" << 12.5 << 12.0 << "Hiddne acc" << "blue";
}

void
TestAccountsMocked::testStoreAccount() {
    QFETCH(QString, name);
    QFETCH(double, amount);
    QFETCH(double, initialAmount);
    QFETCH(QString, memo);
    QFETCH(QString, color);

    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();
    auto acc = std::make_shared<chancho::Account>(name, amount, memo);
    acc->initialAmount = initialAmount;
    acc->color = color;

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillOnce(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(INSERT_UPDATE_ACCOUNT)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":uuid"),
                          Matcher<const QVariant&>(_),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":name"),
                          Matcher<const QVariant&>(acc->name),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":memo"),
                          Matcher<const QVariant&>(acc->memo),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":color"),
                          Matcher<const QVariant&>(acc->color),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":initialAmount"),
                          Matcher<const QVariant&>(QString::number(acc->initialAmount)),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":amount"),
                          Matcher<const QVariant&>(QString::number(acc->amount)),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*db.get(), lastError())
            .Times(0);

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;

    book.store(acc);
    QVERIFY(!book.isError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testStoreAccountListTransactionError() {
    QSqlError error(QString(QTest::currentTestFunction()), "Test Error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                                         Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), transaction())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    QList<com::chancho::AccountPtr> accs {
        std::make_shared<chancho::Account>("Bankia", 34.3, "Savings"),
        std::make_shared<chancho::Account>("BBVA", 0.3, "International")
    };

    PublicBook book;
    book.store(accs);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestAccountsMocked::testStoreAccountListExecError() {
    QSqlError error(QString(QTest::currentTestFunction()), "Test Error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), transaction())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillOnce(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(INSERT_UPDATE_ACCOUNT)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(_),
                          Matcher<const QVariant&>(_),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), rollback())
            .Times(1);

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    QList<com::chancho::AccountPtr> accs {
        std::make_shared<chancho::Account>("Bankia", 34.3, "Savings"),
        std::make_shared<chancho::Account>("BBVA", 0.3, "International")
    };

    PublicBook book;
    book.store(accs);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testStoreAccountList_data() {
    QTest::addColumn<QList<chancho::AccountPtr>>("accounts");

    QList<chancho::AccountPtr> emptyList;
    QList<chancho::AccountPtr> singleObjList{
        std::make_shared<chancho::Account>("Bankia", 34.3, "Savings")
    };
    singleObjList.at(0)->initialAmount = 9.8;
    singleObjList.at(0)->color = "white";

    QList<chancho::AccountPtr> severalObjList{
            std::make_shared<chancho::Account>("BBVA", 0.3, "International"),
            std::make_shared<chancho::Account>("Bankia", 34.3, "Savings"),
            std::make_shared<chancho::Account>("ING", 34.9, "")
    };

    severalObjList.at(0)->initialAmount = 9.8;
    severalObjList.at(0)->color = "white";
    severalObjList.at(1)->initialAmount = 9.67;
    severalObjList.at(1)->color = "black";
    severalObjList.at(2)->initialAmount = 0.8;
    severalObjList.at(2)->color = "blue";

    QTest::newRow("empty-list") << emptyList;
    QTest::newRow("single-obj-list") << singleObjList;
    QTest::newRow("several-obj-list") << severalObjList;
}

void
TestAccountsMocked::testStoreAccountList() {
    QFETCH(QList<chancho::AccountPtr>, accounts);
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    if (accounts.count() > 0) {
        EXPECT_CALL(*db.get(), open())
                .Times(1)
                .WillOnce(Return(true));
    } else {
        EXPECT_CALL(*db.get(), open())
                .Times(0);
    }

    if (accounts.count() > 0) {
        EXPECT_CALL(*db.get(), transaction())
                .Times(1)
                .WillOnce(Return(true));
    } else {
        EXPECT_CALL(*db.get(), transaction())
                .Times(0);
    }

    if (accounts.count() > 0) {
        EXPECT_CALL(*db.get(), createQuery())
                .Times(accounts.count())
                .WillRepeatedly(Return(query));
    } else {
        EXPECT_CALL(*db.get(), createQuery())
                .Times(0);
    }

    if (accounts.count() > 0) {
        EXPECT_CALL(*query.get(), prepare(QStringEqual(INSERT_UPDATE_ACCOUNT)))
                .Times(accounts.count())
                .WillRepeatedly(Return(true));
    } else {
        EXPECT_CALL(*query.get(), prepare(QStringEqual(INSERT_UPDATE_ACCOUNT)))
                .Times(0);
    }

    if (accounts.count() > 0) {
        EXPECT_CALL(*query.get(),
                    bindValue(Matcher<const QString &>(":uuid"),
                              Matcher<const QVariant &>(_),
                              Matcher < QFlags < QSql::ParamTypeFlag >> (_)))
                .Times(accounts.count());
    }

    foreach(const chancho::AccountPtr& acc, accounts) {
        EXPECT_CALL(*query.get(),
                    bindValue(Matcher<const QString&>(":name"),
                              Matcher<const QVariant&>(acc->name),
                              Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
                .Times(1);

        EXPECT_CALL(*query.get(),
                    bindValue(Matcher<const QString&>(":memo"),
                              Matcher<const QVariant&>(acc->memo),
                              Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
                .Times(1);

        EXPECT_CALL(*query.get(),
                    bindValue(Matcher<const QString&>(":color"),
                              Matcher<const QVariant&>(acc->color),
                              Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
                .Times(1);

        EXPECT_CALL(*query.get(),
                    bindValue(Matcher<const QString&>(":initialAmount"),
                              Matcher<const QVariant&>(QString::number(acc->initialAmount)),
                              Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
                .Times(1);

        EXPECT_CALL(*query.get(),
                    bindValue(Matcher<const QString&>(":amount"),
                              Matcher<const QVariant&>(QString::number(acc->amount)),
                              Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
                .Times(1);
    }

    EXPECT_CALL(*db.get(), lastError())
            .Times(0);

    if (accounts.count() > 0) {
        EXPECT_CALL(*query.get(), exec())
                .Times(accounts.count())
                .WillRepeatedly(Return(true));
    } else {
        EXPECT_CALL(*query.get(), exec())
                .Times(0);
    }

    if (accounts.count() > 0) {
        EXPECT_CALL(*db.get(), commit())
                .Times(1);
    } else {
        EXPECT_CALL(*db.get(), commit())
                .Times(0);
    }

    if (accounts.count() > 0) {
        EXPECT_CALL(*db.get(), close())
                .Times(1);
    } else {
        EXPECT_CALL(*db.get(), close())
                .Times(0);
    }

    PublicBook book;
    book.store(accounts);

    QVERIFY(!book.isError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testRemoveAccountNotStored() {
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(0);

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    auto acc = std::make_shared<PublicAccount>("Bankia", 23.2, "Memo");
    PublicBook book;

    book.remove(acc);
    QVERIFY(book.isError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestAccountsMocked::testRemoveAccountExecError() {
    QSqlError error(QString(QTest::currentTestFunction()), "Test Error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();
    auto acc = std::make_shared<PublicAccount>("Bankia", 23.2, "Memo");
    acc->_dbId = QUuid::createUuid();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillOnce(Return(query));

    // child query expectations
    EXPECT_CALL(*query.get(), prepare(DELETE_ACCOUNT))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(_),
                          Matcher<const QVariant&>(acc->_dbId.toString()),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);


    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    // delete query expectations
    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    book.remove(acc);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testRemoveAccount_data() {
    QTest::addColumn<PublicAccountPtr>("account");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 23.2, "Memo");
    firstAcc->_dbId = QUuid::createUuid();

    auto secondAcc = std::make_shared<PublicAccount>("Bankia", 23.2, "Memo");
    secondAcc->_dbId = QUuid::createUuid();

    auto lastAcc = std::make_shared<PublicAccount>("Bankia", 23.2, "Memo");
    lastAcc->_dbId = QUuid::createUuid();

    QTest::newRow("first-acc") << firstAcc;
    QTest::newRow("second-acc") << secondAcc;
    QTest::newRow("last-acc") << lastAcc;
}

void
TestAccountsMocked::testRemoveAccount() {
    QFETCH(PublicAccountPtr, account);

    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillOnce(Return(query));

    // child query expectations
    EXPECT_CALL(*query.get(), prepare(DELETE_ACCOUNT))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(_),
                          Matcher<const QVariant&>(account->_dbId.toString()),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);


    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    book.remove(account);

    QVERIFY(!book.isError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testAccountsNoLimitNoOffset_data() {
    QTest::addColumn<QList<AccountDbRow>>("rows");

    QList<AccountDbRow> emptyResult;
    QList<AccountDbRow> oneResult {
        AccountDbRow { QUuid::createUuid(), "Bankia", "Savings", "white", 3.4, 90.0}
    };
    QList<AccountDbRow> severalResults {
        AccountDbRow { QUuid::createUuid(), "Bankia", "Savings", "white", 3.4, 90.0},
        AccountDbRow { QUuid::createUuid(), "ING", "Secret", "blue", .9, 9.0},
        AccountDbRow { QUuid::createUuid(), "BBVA", "", "black", 3.49, 190.0},
    };

    QTest::newRow("empty") << emptyResult;
    QTest::newRow("single") << oneResult;
    QTest::newRow("multiple") << severalResults;
}

void
TestAccountsMocked::testAccountsNoLimitNoOffset() {
    QFETCH(QList<AccountDbRow>, rows);

    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillRepeatedly(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ALL_ACCOUNTS)))
            .Times(1)
            .WillRepeatedly(Return(true));

    Sequence sequence;
    foreach(const AccountDbRow& row, rows) {
        EXPECT_CALL(*query.get(), value(0))
                .InSequence(sequence)
                .WillOnce(Return(row.uuid));

        EXPECT_CALL(*query.get(), value(1))
                .InSequence(sequence)
                .WillOnce(Return(row.name));

        EXPECT_CALL(*query.get(), value(2))
                .InSequence(sequence)
                .WillOnce(Return(row.memo));

        EXPECT_CALL(*query.get(), value(3))
                .InSequence(sequence)
                .WillOnce(Return(row.color));

        EXPECT_CALL(*query.get(), value(4))
                .InSequence(sequence)
                .WillOnce(Return(row.initialAmount));

        EXPECT_CALL(*query.get(), value(5))
                .InSequence(sequence)
                .WillOnce(Return(row.amount));
    }

    EXPECT_CALL(*db.get(), lastError())
            .Times(0);

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillRepeatedly(Return(true));

    if (rows.count() > 0) {
        // create a sequence making sure we return true everytime but the last one
        Sequence nextSequence;
        for(int index = 0; index <= rows.count(); index++) {
            if (index == rows.count()) {
                EXPECT_CALL(*query.get(), next())
                        .InSequence(nextSequence)
                        .WillOnce(Return(false));
            } else {
                EXPECT_CALL(*query.get(), next())
                        .InSequence(nextSequence)
                        .WillOnce(Return(true));
            }
        }
    } else {
        EXPECT_CALL(*query.get(), next())
                .Times(1)
                .WillOnce(Return(false));
    }

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto accounts = book.accounts();

    QVERIFY(!book.isError());
    QCOMPARE(accounts.count(), rows.count());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testAccountsNoLimitNoOffsetError() {
    QSqlError error(QString(QTest::currentTestFunction()), "Test Error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillRepeatedly(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ALL_ACCOUNTS)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillRepeatedly(Return(false));

    EXPECT_CALL(*query.get(), next())
            .Times(0);

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto accounts = book.accounts();

    QVERIFY(book.isError());
    QCOMPARE(accounts.count(), 0);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testAccountsLimitNoOffset_data() {
    QTest::addColumn<QList<AccountDbRow>>("rows");

    QList<AccountDbRow> emptyResult;
    QList<AccountDbRow> oneResult {
            AccountDbRow { QUuid::createUuid(), "Bankia", "Savings", "white", 3.4, 90.0}
    };
    QList<AccountDbRow> severalResults {
            AccountDbRow { QUuid::createUuid(), "Bankia", "Savings", "white", 3.4, 90.0},
            AccountDbRow { QUuid::createUuid(), "ING", "Secret", "blue", .9, 9.0},
            AccountDbRow { QUuid::createUuid(), "BBVA", "", "black", 3.49, 190.0},
    };

    QTest::newRow("empty") << emptyResult;
    QTest::newRow("single") << oneResult;
    QTest::newRow("multiple") << severalResults;
}

void
TestAccountsMocked::testAccountsLimitNoOffset() {
    QFETCH(QList<AccountDbRow>, rows);

    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillRepeatedly(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ALL_ACCOUNTS_LIMIT)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":limit"),
                                        Matcher<const QVariant&>(QVariant(rows.count())),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":offset"),
                                        Matcher<const QVariant&>(QVariant(0)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    Sequence sequence;
    foreach(const AccountDbRow& row, rows) {
        EXPECT_CALL(*query.get(), value(0))
                .InSequence(sequence)
                .WillOnce(Return(row.uuid));

        EXPECT_CALL(*query.get(), value(1))
                .InSequence(sequence)
                .WillOnce(Return(row.name));

        EXPECT_CALL(*query.get(), value(2))
                .InSequence(sequence)
                .WillOnce(Return(row.memo));

        EXPECT_CALL(*query.get(), value(3))
                .InSequence(sequence)
                .WillOnce(Return(row.color));

        EXPECT_CALL(*query.get(), value(4))
                .InSequence(sequence)
                .WillOnce(Return(row.initialAmount));

        EXPECT_CALL(*query.get(), value(5))
                .InSequence(sequence)
                .WillOnce(Return(row.amount));
    }

    EXPECT_CALL(*db.get(), lastError())
            .Times(0);

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillRepeatedly(Return(true));

    if (rows.count() > 0) {
        // create a sequence making sure we return true everytime but the last one
        Sequence nextSequence;
        for(int index = 0; index <= rows.count(); index++) {
            if (index == rows.count()) {
                EXPECT_CALL(*query.get(), next())
                        .InSequence(nextSequence)
                        .WillOnce(Return(false));
            } else {
                EXPECT_CALL(*query.get(), next())
                        .InSequence(nextSequence)
                        .WillOnce(Return(true));
            }
        }
    } else {
        EXPECT_CALL(*query.get(), next())
                .Times(1)
                .WillOnce(Return(false));
    }

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto accounts = book.accounts(rows.count());

    QVERIFY(!book.isError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testAccountsLimitNoOffsetError() {
    auto limit = 4;
    QSqlError error(QString(QTest::currentTestFunction()), "Test Error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillRepeatedly(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ALL_ACCOUNTS_LIMIT)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":limit"),
                                        Matcher<const QVariant&>(QVariant(limit)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":offset"),
                                        Matcher<const QVariant&>(QVariant(0)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillRepeatedly(Return(false));

    EXPECT_CALL(*query.get(), next())
            .Times(0);

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto accounts = book.accounts(limit);

    QVERIFY(book.isError());
    QCOMPARE(accounts.count(), 0);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testAccountsLimitOffset_data() {
    QTest::addColumn<QList<AccountDbRow>>("rows");
    QTest::addColumn<int>("limit");
    QTest::addColumn<int>("offset");

    QList<AccountDbRow> emptyResult;
    QList<AccountDbRow> oneResult {
            AccountDbRow { QUuid::createUuid(), "Bankia", "Savings", "white", 3.4, 90.0}
    };
    QList<AccountDbRow> severalResults {
            AccountDbRow { QUuid::createUuid(), "Bankia", "Savings", "white", 3.4, 90.0},
            AccountDbRow { QUuid::createUuid(), "ING", "Secret", "blue", .9, 9.0},
            AccountDbRow { QUuid::createUuid(), "BBVA", "", "black", 3.49, 190.0},
    };

    QTest::newRow("empty") << emptyResult << 4 << 2;
    QTest::newRow("single") << oneResult << 5 << 1;
    QTest::newRow("multiple") << severalResults << 9 << 3;
}

void
TestAccountsMocked::testAccountsLimitOffset() {
    QFETCH(QList<AccountDbRow>, rows);
    QFETCH(int, limit);
    QFETCH(int, offset);

    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillRepeatedly(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ALL_ACCOUNTS_LIMIT)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":limit"),
                                        Matcher<const QVariant&>(QVariant(limit)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":offset"),
                                        Matcher<const QVariant&>(QVariant(offset)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    Sequence sequence;
    foreach(const AccountDbRow& row, rows) {
        EXPECT_CALL(*query.get(), value(0))
                .InSequence(sequence)
                .WillOnce(Return(row.uuid));

        EXPECT_CALL(*query.get(), value(1))
                .InSequence(sequence)
                .WillOnce(Return(row.name));

        EXPECT_CALL(*query.get(), value(2))
                .InSequence(sequence)
                .WillOnce(Return(row.memo));

        EXPECT_CALL(*query.get(), value(3))
                .InSequence(sequence)
                .WillOnce(Return(row.color));

        EXPECT_CALL(*query.get(), value(4))
                .InSequence(sequence)
                .WillOnce(Return(row.initialAmount));

        EXPECT_CALL(*query.get(), value(5))
                .InSequence(sequence)
                .WillOnce(Return(row.amount));
    }

    EXPECT_CALL(*db.get(), lastError())
            .Times(0);

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillRepeatedly(Return(true));

    if (rows.count() > 0) {
        // create a sequence making sure we return true everytime but the last one
        Sequence nextSequence;
        for(int index = 0; index <= rows.count(); index++) {
            if (index == rows.count()) {
                EXPECT_CALL(*query.get(), next())
                        .InSequence(nextSequence)
                        .WillOnce(Return(false));
            } else {
                EXPECT_CALL(*query.get(), next())
                        .InSequence(nextSequence)
                        .WillOnce(Return(true));
            }
        }
    } else {
        EXPECT_CALL(*query.get(), next())
                .Times(1)
                .WillOnce(Return(false));
    }

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto accounts = book.accounts(limit, offset);

    QVERIFY(!book.isError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testAccountsLimitOffsetError() {
    auto limit = 4;
    auto offset = 3;
    QSqlError error(QString(QTest::currentTestFunction()), "Test Error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillRepeatedly(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ALL_ACCOUNTS_LIMIT)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":limit"),
                                        Matcher<const QVariant&>(QVariant(limit)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":offset"),
                                        Matcher<const QVariant&>(QVariant(offset)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillRepeatedly(Return(false));

    EXPECT_CALL(*query.get(), next())
            .Times(0);

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto accounts = book.accounts(limit, offset);

    QVERIFY(book.isError());
    QCOMPARE(accounts.count(), 0);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testNumberOfAccountsExecError() {
    QSqlError error(QString(QTest::currentTestFunction()), "Test Error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillRepeatedly(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ACCOUNTS_COUNT)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillRepeatedly(Return(false));

    EXPECT_CALL(*query.get(), next())
            .Times(0);

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto count = book.numberOfAccounts();

    QVERIFY(book.isError());
    QCOMPARE(count, -1);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestAccountsMocked::testNumberOfAccountsExec_data() {
    QTest::addColumn<int>("dbCount");

    QTest::newRow("first") << 2;
    QTest::newRow("second") << 12;
    QTest::newRow("last") << 44;
}

void
TestAccountsMocked::testNumberOfAccountsExec() {
    QFETCH(int, dbCount);

    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

    // set db interaction expectations
    // set db interaction expectations
    EXPECT_CALL(*_dbFactory,
                addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")),
                            Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillRepeatedly(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ACCOUNTS_COUNT)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*query.get(), value(0))
            .Times(1)
            .WillOnce(Return(dbCount));

    EXPECT_CALL(*db.get(), lastError())
            .Times(0);

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*query.get(), next())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto count = book.numberOfAccounts();

    QVERIFY(!book.isError());
    QCOMPARE(dbCount, count);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

}
}
}

QTEST_MAIN(com::chancho::tests::TestAccountsMocked)


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
#include <QtSql/qsqlquerymodel.h>
#include <QtSql/qsqltablemodel.h>

#include "matchers.h"
#include "public_account.h"
#include "public_transaction.h"
#include "test_book_mocked.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;

void
TestBookMocked::init() {
    BaseTestCase::init();
    _dbFactory = new tests::MockDatabaseFactory();

    chancho::system::DatabaseFactory::setInstance(_dbFactory);
}

void
TestBookMocked::cleanup() {
    BaseTestCase::cleanup();
    QFileInfo fi(PublicBook::databasePath());
    if (fi.exists()) {
        QFile::remove(PublicBook::databasePath());
    }
    chancho::system::DatabaseFactory::deleteInstance();
}

void
TestBookMocked::testDatabasePathMissing() {
    // ensure that the phone is not present
    auto dbPath = QStandardPaths::locate(QStandardPaths::DataLocation, "chancho.db");
    QVERIFY(dbPath.isEmpty());

    auto createdPath = PublicBook::databasePath();
    QVERIFY(!createdPath.isEmpty());
    QFileInfo fi(createdPath);
    QVERIFY(QFile::exists(fi.path()));
    QVERIFY(!fi.exists());
}

void
TestBookMocked::testInitDatbaseMissingTables() {
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();
    QStringList tables;

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
        .Times(1)
        .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
        .Times(1);

    EXPECT_CALL(*db.get(), open())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), tables(_))
        .Times(1)
        .WillOnce(Return(tables));

    EXPECT_CALL(*db.get(), transaction())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
        .Times(1)
        .WillOnce(Return(query));

    EXPECT_CALL(*query.get(), exec(Matcher<const QString&>(_)))
        .Times(13)
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*db.get(), commit())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), close())
        .Times(1);

    PublicBook::initDatabse();

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestBookMocked::testInitDatbaseMissingTablesError() {
    QStringList tables;
    QSqlError err;
    auto query = std::make_shared<tests::MockQuery>();
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
        .Times(1)
        .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
        .Times(1);

    EXPECT_CALL(*db.get(), open())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), tables(_))
        .Times(1)
        .WillOnce(Return(tables));

    EXPECT_CALL(*db.get(), transaction())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
        .Times(1)
        .WillOnce(Return(query));

    EXPECT_CALL(*query.get(), exec(Matcher<const QString&>(_)))
        .Times(13)
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), commit())
        .Times(0);

    EXPECT_CALL(*db.get(), rollback())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), lastError())
        .Times(1)
        .WillOnce(Return(err));

    EXPECT_CALL(*db.get(), close())
        .Times(1);

    PublicBook::initDatabse();

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestBookMocked::testInitDatabasePresentTables_data() {
    QTest::addColumn<QStringList>("tables");

    QStringList first;
    first.append("Categories");

    QStringList second;
    second.append("CATEGORIES");

    QStringList third;
    third.append("CateGORIES");

    QTest::newRow("first-obj") << first;
    QTest::newRow("second-obj") << second;
    QTest::newRow("last-obj") << third;
}

void
TestBookMocked::testInitDatabasePresentTables() {
    QFETCH(QStringList, tables);
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
        .Times(1)
        .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
        .Times(1);

    EXPECT_CALL(*db.get(), open())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), tables(_))
        .Times(1)
        .WillOnce(Return(tables));

    EXPECT_CALL(*db.get(), close())
        .Times(1);

    PublicBook::initDatabse();

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testStoreCategoryOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
        .Times(1)
        .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
        .Times(1);

    EXPECT_CALL(*db.get(), open())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
        .Times(1)
        .WillOnce(Return(error));

    auto cat = std::make_shared<chancho::Category>("Testing cate", chancho::Category::Type::INCOME);
    PublicBook book;

    book.store(cat);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testStoreCategoryExecError() {
    QSqlError error("Driver error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

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

    EXPECT_CALL(*query.get(), prepare(_))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
        .Times(AnyNumber());

    EXPECT_CALL(*db.get(), lastError())
        .Times(1)
        .WillOnce(Return(error));

    EXPECT_CALL(*query.get(), exec())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), close())
        .Times(1);

    auto cat = std::make_shared<chancho::Category>("Testing cate", chancho::Category::Type::INCOME);
    PublicBook book;

    book.store(cat);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestBookMocked::testRemoveCategoryOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
        .Times(1)
        .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
        .Times(1);

    EXPECT_CALL(*db.get(), open())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
        .Times(1)
        .WillOnce(Return(error));

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

    PublicBook book;

    book.remove(cat);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(cat.get()));
}

void
TestBookMocked::testRemoveCategoryChildsExecError() {
    QSqlError error("Driver error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto deleteChildsQuery = std::make_shared<tests::MockQuery>();
    auto deleteQuery = std::make_shared<tests::MockQuery>();

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

    EXPECT_CALL(*db.get(), transaction())  // a transaction should be started
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
        .Times(2)
        .WillOnce(Return(deleteChildsQuery))
        .WillOnce(Return(deleteQuery));

    // child query expectations
    EXPECT_CALL(*deleteChildsQuery.get(), prepare(_))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*deleteChildsQuery.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
        .Times(AnyNumber());


    EXPECT_CALL(*deleteChildsQuery.get(), exec())
        .Times(1)
        .WillOnce(Return(false));

    // delete query expectations
    EXPECT_CALL(*deleteQuery.get(), prepare(_))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*deleteQuery.get(),
        bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
        .Times(AnyNumber());


    EXPECT_CALL(*deleteQuery.get(), exec())
        .Times(1)
        .WillOnce(Return(true));  // should not matter since the error is comming from a previous query

    EXPECT_CALL(*db.get(), lastError())
        .Times(1)
        .WillOnce(Return(error));

    EXPECT_CALL(*db.get(), rollback()) // should rollback other queries
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), commit()) // never commit
        .Times(0);

    EXPECT_CALL(*db.get(), close())
        .Times(1);

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();
    PublicBook book;

    book.remove(cat);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(deleteChildsQuery.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(deleteQuery.get()));
}

void
TestBookMocked::testRemoveCategoryDeleteExecError() {
    QSqlError error("Driver error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto deleteChildsQuery = std::make_shared<tests::MockQuery>();
    auto deleteQuery = std::make_shared<tests::MockQuery>();

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

    EXPECT_CALL(*db.get(), transaction())  // a transaction should be started
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(2)
            .WillOnce(Return(deleteChildsQuery))
            .WillOnce(Return(deleteQuery));

    // child query expectations
    EXPECT_CALL(*deleteChildsQuery.get(), prepare(_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*deleteChildsQuery.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());


    EXPECT_CALL(*deleteChildsQuery.get(), exec())
            .Times(1)
            .WillOnce(Return(true));  // testing the error with the other query

    // delete query expectations
    EXPECT_CALL(*deleteQuery.get(), prepare(_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*deleteQuery.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());


    EXPECT_CALL(*deleteQuery.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*db.get(), rollback()) // should rollback other queries
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), commit()) // never commit
            .Times(0);

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();
    PublicBook book;

    book.remove(cat);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(deleteChildsQuery.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(deleteQuery.get()));
}

void
TestBookMocked::testStoreAccountsOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    auto acc = std::make_shared<chancho::Account>("Bankia", 34.3, "Savings");
    PublicBook book;

    book.store(acc);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testStoreAccountExecError() {
    QSqlError error("Driver error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

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

    EXPECT_CALL(*query.get(), prepare(_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    auto acc = std::make_shared<chancho::Account>("Bankia", 53.23, "Savings");
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
TestBookMocked::testRemoveAccountOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    auto acc = std::make_shared<PublicAccount>("Bankia", 232.32, "Savings");
    acc->_dbId = QUuid::createUuid();

    PublicBook book;

    book.remove(acc);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testRemoveAccountExecError() {
    QSqlError error("Driver error");
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
    EXPECT_CALL(*query.get(), prepare(_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());


    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    // delete query expectations
    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    auto acc = std::make_shared<PublicAccount>("Bankia", 23.2, "Memo");
    acc->_dbId = QUuid::createUuid();
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
TestBookMocked::testAccountsOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    PublicBook book;

    auto accounts = book.accounts();
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());
    QCOMPARE(accounts.count(), 0);

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testAccountsExecError() {
    QSqlError error("Driver error");
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

    EXPECT_CALL(*query.get(), exec(_))
            .Times(1)
            .WillOnce(Return(false));

    // delete query expectations
    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto accounts = book.accounts();
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());
    QCOMPARE(accounts.count(), 0);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestBookMocked::testCategoriesOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    PublicBook book;

    auto categories = book.categories();
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());
    QCOMPARE(categories.count(), 0);

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testCategoriesExecError() {
    QSqlError error("Driver error");
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

    EXPECT_CALL(*query.get(), exec(_))
            .Times(1)
            .WillOnce(Return(false));

    // delete query expectations
    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto categories = book.categories();
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());
    QCOMPARE(categories.count(), 0);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestBookMocked::testStoreTransactionOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    auto acc = std::make_shared<PublicAccount>("Bankia", 232.32, "Savings");
    acc->_dbId = QUuid::createUuid();

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

    auto tran = std::make_shared<chancho::Transaction>(acc, 34.4, cat);

    PublicBook book;
    book.store(tran);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testStoreTransactionExecError() {
    QSqlError error("Driver error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();

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

    EXPECT_CALL(*query.get(), prepare(_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    auto acc = std::make_shared<PublicAccount>("Bankia", 232.32, "Savings");
    acc->_dbId = QUuid::createUuid();

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

    auto tran = std::make_shared<chancho::Transaction>(acc, 34.4, cat);

    PublicBook book;
    book.store(tran);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestBookMocked::testRemoveTransactionOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    auto acc = std::make_shared<PublicAccount>("Bankia", 232.32, "Savings");
    acc->_dbId = QUuid::createUuid();

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

    auto tran = std::make_shared<PublicTransaction>(acc, 34.4, cat);
    tran->_dbId = QUuid::createUuid();

    PublicBook book;

    book.remove(tran);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(tran.get()));
}

void
TestBookMocked::testRemoveTransactionExecError() {
    QSqlError error("Driver error");
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

    EXPECT_CALL(*query.get(), prepare(_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    // delete query expectations
    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    auto acc = std::make_shared<PublicAccount>("Bankia", 232.32, "Savings");
    acc->_dbId = QUuid::createUuid();

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

    auto tran = std::make_shared<PublicTransaction>(acc, 34.4, cat);
    tran->_dbId = QUuid::createUuid();

    PublicBook book;
    book.remove(tran);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestBookMocked::testTransactionsMonthOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    PublicBook book;
    book.transactions(2, 2014);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testTransactionsMonthExecError() {
    QSqlError error("Driver error");
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

    EXPECT_CALL(*query.get(), prepare(_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());

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
    auto trans = book.transactions(2, 2015);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());
    QCOMPARE(trans.count(), 0);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestBookMocked::testTransactionsCategoryOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

    PublicBook book;
    book.transactions(cat);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testTransactionsCategoryExecError() {
    QSqlError error("Driver error");
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

    EXPECT_CALL(*query.get(), prepare(_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    // delete query expectations
    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

    PublicBook book;
    auto trans = book.transactions(cat);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());
    QCOMPARE(trans.count(), 0);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestBookMocked::testTransactionsCategoryMonthOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

    PublicBook book;
    book.transactions(cat, 2, 2015);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testTransactionsCategoryMonthExecError() {
    QSqlError error("Driver error");
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

    EXPECT_CALL(*query.get(), prepare(_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    // delete query expectations
    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    auto cat = std::make_shared<PublicCategory>("Testing cate", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

    PublicBook book;
    auto trans = book.transactions(cat, 1, 2015);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());
    QCOMPARE(trans.count(), 0);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestBookMocked::testTransactionsAccountOpenError() {
    QSqlError error("Testing driver error", "Text error");
    auto db = std::make_shared<tests::MockDatabase>();

    // set db interaction expectations
    EXPECT_CALL(*_dbFactory, addDatabase(Matcher<const QString&>(QStringEqual("QSQLITE")), Matcher<const QString&>(QStringEqual("BOOKS"))))
            .Times(1)
            .WillOnce(Return(db));

    EXPECT_CALL(*db.get(), setDatabaseName(QStringEqual(PublicBook::databasePath())))
            .Times(1);

    EXPECT_CALL(*db.get(), open())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    auto acc = std::make_shared<PublicAccount>("Bankia", 232.32, "Savings");
    acc->_dbId = QUuid::createUuid();

    PublicBook book;
    book.transactions(acc);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestBookMocked::testTransactionsAccountExecError() {
    QSqlError error("Driver error");
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

    EXPECT_CALL(*query.get(), prepare(_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
            bindValue(Matcher<const QString&>(_), Matcher<const QVariant&>(_), Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(AnyNumber());

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    // delete query expectations
    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    auto acc = std::make_shared<PublicAccount>("Bankia", 232.32, "Savings");
    acc->_dbId = QUuid::createUuid();

    PublicBook book;
    auto trans = book.transactions(acc);
    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());
    QCOMPARE(trans.count(), 0);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

QTEST_MAIN(TestBookMocked)

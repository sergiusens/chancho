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
#include "test_categories_mocked.h"

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
    const QString INSERT_CATEGORY = "INSERT INTO Categories(uuid, parent, name, type, color) " \
        "VALUES (:uuid, :parent, :name, :type, :color)";
    const QString UPDATE_CATEGORY = "UPDATE Categories SET parent=:parent, name=:name, type=:type, color=:color " \
        "WHERE uuid=:uuid";
    const QString DELETE_CATEGORY = "DELETE FROM Categories WHERE uuid=:uuid";
    const QString SELECT_ALL_CATEGORIES = "SELECT uuid, parent, name, type, color FROM Categories ORDER BY name ASC";
    const QString SELECT_ALL_CATEGORIES_LIMIT = "SELECT uuid, parent, name, type, color FROM Categories ORDER BY name ASC "\
        "LIMIT :limit OFFSET :offset";
    const QString SELECT_ALL_CATEGORIES_TYPE = "SELECT uuid, parent, name, type, color FROM Categories WHERE type=:type "\
        "ORDER BY name ASC";
    const QString SELECT_ALL_CATEGORIES_TYPE_LIMIT = "SELECT uuid, parent, name, type, color FROM Categories "
            "WHERE type=:type ORDER BY name ASC LIMIT :limit OFFSET :offset";
    const QString SELECT_CATEGORIES_COUNT = "SELECT count(*) FROM Categories";
    const QString SELECT_CATEGORIES_COUNT_TYPE = "SELECT count(*) FROM Categories WHERE type=:type";
    const QString SELECT_CATEGORIES_RECURRENT = "SELECT uuid, parent, name, type, color FROM Categories WHERE uuid IN "\
        "(SELECT category from RecurrentTransactions GROUP BY category) ORDER BY name";
    const QString SELECT_CATEGORIES_RECURRENT_LIMIT = "SELECT uuid, parent, name, type, color FROM Categories WHERE uuid IN "\
        "(SELECT category from RecurrentTransactions GROUP BY category) ORDER BY name LIMIT :limit OFFSET :offset";
    const QString SELECT_CATEGORIES_RECURRENT_COUNT = "SELECT count(*) FROM (SELECT category FROM "\
        "RecurrentTransactions GROUP BY category)";
}

void
TestCategoriesMocked::init() {
    BaseTestCase::init();
    _dbFactory = new tests::MockDatabaseFactory();

    chancho::system::DatabaseFactory::setInstance(_dbFactory);
}

void
TestCategoriesMocked::cleanup() {
    BaseTestCase::cleanup();
    QFileInfo fi(PublicBook::databasePath());
    if (fi.exists()) {
        QFile::remove(PublicBook::databasePath());
    }
    chancho::system::DatabaseFactory::deleteInstance();
}

void
TestCategoriesMocked::testStoreCategoryNew_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<chancho::Category::Type>("type");
    QTest::addColumn<QString>("color");

    QTest::newRow("first-cat") << "Salary" << chancho::Category::Type::INCOME << "white";
    QTest::newRow("second-cat") << "Gas" << chancho::Category::Type::EXPENSE << "reb";
    QTest::newRow("last-cat") << "Diesel" << chancho::Category::Type::EXPENSE << "black";
}

void
TestCategoriesMocked::testStoreCategoryNew() {
    QFETCH(QString, name);
    QFETCH(chancho::Category::Type, type);
    QFETCH(QString, color);


    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();
    auto cat = std::make_shared<PublicCategory>(name, type);
    cat->color = color;

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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(INSERT_CATEGORY)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":uuid"),
                          Matcher<const QVariant&>(_),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":name"),
                          Matcher<const QVariant&>(cat->name),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":parent"),
                          Matcher<const QVariant&>(QVariant()),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":type"),
                          Matcher<const QVariant&>(static_cast<int>(cat->type)),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":color"),
                          Matcher<const QVariant&>(cat->color),
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
    book.store(cat);
    QVERIFY(!book.isError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testStoreCategoryUpdate_data() {
    QTest::addColumn<QUuid>("uuid");
    QTest::addColumn<QString>("name");
    QTest::addColumn<chancho::Category::Type>("type");
    QTest::addColumn<QString>("color");

    QTest::newRow("first-cat") << QUuid::createUuid() << "Salary" << chancho::Category::Type::INCOME << "white";
    QTest::newRow("second-cat") << QUuid::createUuid() << "Gas" << chancho::Category::Type::EXPENSE << "reb";
    QTest::newRow("last-cat") << QUuid::createUuid() << "Diesel" << chancho::Category::Type::EXPENSE << "black";
}

void
TestCategoriesMocked::testStoreCategoryUpdate() {
    QFETCH(QUuid, uuid);
    QFETCH(QString, name);
    QFETCH(chancho::Category::Type, type);
    QFETCH(QString, color);


    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();
    auto cat = std::make_shared<PublicCategory>(name, type);
    cat->color = color;
    cat->_dbId = uuid;

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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(UPDATE_CATEGORY)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":uuid"),
                          Matcher<const QVariant&>(uuid.toString()),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":name"),
                          Matcher<const QVariant&>(cat->name),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":parent"),
                          Matcher<const QVariant&>(QVariant()),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":type"),
                          Matcher<const QVariant&>(static_cast<int>(cat->type)),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":color"),
                          Matcher<const QVariant&>(cat->color),
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
    book.store(cat);
    QVERIFY(!book.isError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testStoreCategoryExecError() {
    QSqlError error(QString(QTest::currentTestFunction()), "Test Error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();
    auto cat = std::make_shared<PublicCategory>("Test", chancho::Category::Type::INCOME);
    cat->color = "back";

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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(INSERT_CATEGORY)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":uuid"),
                          Matcher<const QVariant&>(_),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":name"),
                          Matcher<const QVariant&>(cat->name),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":parent"),
                          Matcher<const QVariant&>(QVariant()),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":type"),
                          Matcher<const QVariant&>(static_cast<int>(cat->type)),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":color"),
                          Matcher<const QVariant&>(cat->color),
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
    book.store(cat);
    QVERIFY(book.isError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testStoreCategoryListTransactionError() {
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

    QList<com::chancho::CategoryPtr> cats {
            std::make_shared<chancho::Category>("Salary", chancho::Category::Type::INCOME),
            std::make_shared<chancho::Category>("Gas", chancho::Category::Type::EXPENSE)
    };

    PublicBook book;
    book.store(cats);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestCategoriesMocked::testStoreCategoryListExecError() {
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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(INSERT_CATEGORY)))
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

    QList<com::chancho::CategoryPtr> cats {
            std::make_shared<chancho::Category>("Salary", chancho::Category::Type::INCOME),
            std::make_shared<chancho::Category>("Gas", chancho::Category::Type::EXPENSE)
    };

    PublicBook book;
    book.store(cats);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testRemoveNotPresent() {
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

    auto cat = std::make_shared<PublicCategory>("Gas", chancho::Category::Type::INCOME);
    PublicBook book;

    book.remove(cat);
    QVERIFY(book.isError());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestCategoriesMocked::testRemoveTransactionError() {
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

    auto cat = std::make_shared<PublicCategory>("Gas", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

    PublicBook book;
    book.remove(cat);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
}

void
TestCategoriesMocked::testRemoveTransactionExecError() {
    QSqlError error(QString(QTest::currentTestFunction()), "Test Error");
    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();
    auto cat = std::make_shared<PublicCategory>("Gas", chancho::Category::Type::INCOME);
    cat->_dbId = QUuid::createUuid();

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
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillOnce(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(DELETE_CATEGORY)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":uuid"),
                          Matcher<const QVariant&>(cat->_dbId),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(*db.get(), rollback())
            .Times(1);

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    PublicBook book;
    book.remove(cat);

    QVERIFY(book.isError());
    QCOMPARE(error.text(), book.lastError());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testRemoveTransaction_data() {
    QTest::addColumn<QUuid>("uuid");
    QTest::addColumn<QString>("name");
    QTest::addColumn<chancho::Category::Type>("type");
    QTest::addColumn<QString>("color");

    QTest::newRow("first-cat") << QUuid::createUuid() << "Salary" << chancho::Category::Type::INCOME << "white";
    QTest::newRow("second-cat") << QUuid::createUuid() << "Gas" << chancho::Category::Type::EXPENSE << "reb";
    QTest::newRow("last-cat") << QUuid::createUuid() << "Diesel" << chancho::Category::Type::EXPENSE << "black";
}

void
TestCategoriesMocked::testRemoveTransaction() {
    QFETCH(QUuid, uuid);
    QFETCH(QString, name);
    QFETCH(chancho::Category::Type, type);
    QFETCH(QString, color);

    auto db = std::make_shared<tests::MockDatabase>();
    auto query = std::make_shared<tests::MockQuery>();
    auto cat = std::make_shared<PublicCategory>(name, type);
    cat->_dbId = uuid;
    cat->color = color;

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
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), createQuery())
            .Times(1)
            .WillOnce(Return(query));

    EXPECT_CALL(*query.get(), prepare(QStringEqual(DELETE_CATEGORY)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":uuid"),
                          Matcher<const QVariant&>(cat->_dbId),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), rollback())
            .Times(0);

    EXPECT_CALL(*db.get(), commit())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    EXPECT_CALL(*db.get(), lastError())
            .Times(0);

    PublicBook book;
    book.remove(cat);

    QVERIFY(!book.isError());
    QVERIFY(!cat->wasStoredInDb());

    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testCategories_data() {
    QTest::addColumn<QList<CategoryDbRow>>("rows");

    //CategoryDbRow(QUuid id, QUuid parent, QString name, chancho::Category::Type  type, QString color)
    QList<CategoryDbRow> emptyResult;
    QList<CategoryDbRow> oneResult {
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Salary", chancho::Category::Type::INCOME, "white"}
    };
    QList<CategoryDbRow> severalResults {
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Gas", chancho::Category::Type::EXPENSE, "red"},
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Food", chancho::Category::Type::EXPENSE, "blue"},
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Allowence", chancho::Category::Type::INCOME, "yellow"},
    };

    QTest::newRow("empty") << emptyResult;
    QTest::newRow("single") << oneResult;
    QTest::newRow("multiple") << severalResults;
}

void
TestCategoriesMocked::testCategories() {
    QFETCH(QList<CategoryDbRow>, rows);

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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ALL_CATEGORIES)))
            .Times(1)
            .WillRepeatedly(Return(true));

    Sequence sequence;
    foreach(const CategoryDbRow& row, rows) {
        EXPECT_CALL(*query.get(), value(0))
               .InSequence(sequence)
               .WillOnce(Return(row.uuid));

       EXPECT_CALL(*query.get(), value(1))
               .InSequence(sequence)
               .WillOnce(Return(row.parent));

       EXPECT_CALL(*query.get(), value(2))
               .InSequence(sequence)
               .WillOnce(Return(row.name));

       EXPECT_CALL(*query.get(), value(3))
               .InSequence(sequence)
               .WillOnce(Return(row.type));

       EXPECT_CALL(*query.get(), value(4))
               .InSequence(sequence)
               .WillOnce(Return(row.color));
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
    auto cats = book.categories();

    QVERIFY(!book.isError());
    QCOMPARE(cats.count(), rows.count());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testCategoriesLimitNoOffset_data() {
    QTest::addColumn<QList<CategoryDbRow>>("rows");
    QTest::addColumn<int>("limit");

    //CategoryDbRow(QUuid id, QUuid parent, QString name, chancho::Category::Type  type, QString color)
    QList<CategoryDbRow> emptyResult;
    QList<CategoryDbRow> oneResult {
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Salary", chancho::Category::Type::INCOME, "white"}
    };
    QList<CategoryDbRow> severalResults {
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Gas", chancho::Category::Type::EXPENSE, "red"},
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Food", chancho::Category::Type::EXPENSE, "blue"},
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Allowence", chancho::Category::Type::INCOME, "yellow"},
    };

    QTest::newRow("empty") << emptyResult << 0;
    QTest::newRow("single") << oneResult << 1;
    QTest::newRow("multiple") << severalResults << 3;
}

void
TestCategoriesMocked::testCategoriesLimitNoOffset() {
    QFETCH(QList<CategoryDbRow>, rows);
    QFETCH(int, limit);

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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ALL_CATEGORIES_TYPE_LIMIT)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":type"),
                                        Matcher<const QVariant&>(_),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":limit"),
                                        Matcher<const QVariant&>(QVariant(limit)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":offset"),
                                        Matcher<const QVariant&>(QVariant(0)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    Sequence sequence;
    foreach(const CategoryDbRow& row, rows) {
        EXPECT_CALL(*query.get(), value(0))
                .InSequence(sequence)
                .WillOnce(Return(row.uuid));

        EXPECT_CALL(*query.get(), value(1))
                .InSequence(sequence)
                .WillOnce(Return(row.parent));

        EXPECT_CALL(*query.get(), value(2))
                .InSequence(sequence)
                .WillOnce(Return(row.name));

        EXPECT_CALL(*query.get(), value(3))
                .InSequence(sequence)
                .WillOnce(Return(row.type));

        EXPECT_CALL(*query.get(), value(4))
                .InSequence(sequence)
                .WillOnce(Return(row.color));
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
    auto cats = book.categories(chancho::Category::Type::INCOME, limit);

    QVERIFY(!book.isError());
    QCOMPARE(cats.count(), rows.count());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testCategoriesLimitOffset_data() {
    QTest::addColumn<QList<CategoryDbRow>>("rows");
    QTest::addColumn<int>("limit");
    QTest::addColumn<int>("offset");

    //CategoryDbRow(QUuid id, QUuid parent, QString name, chancho::Category::Type  type, QString color)
    QList<CategoryDbRow> emptyResult;
    QList<CategoryDbRow> oneResult {
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Salary", chancho::Category::Type::INCOME, "white"}
    };
    QList<CategoryDbRow> severalResults {
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Gas", chancho::Category::Type::EXPENSE, "red"},
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Food", chancho::Category::Type::EXPENSE, "blue"},
            CategoryDbRow { QUuid::createUuid(), QUuid(), "Allowence", chancho::Category::Type::INCOME, "yellow"},
    };

    QTest::newRow("empty") << emptyResult << 0 << 1;
    QTest::newRow("single") << oneResult << 1 << 4;
    QTest::newRow("multiple") << severalResults << 3 << 10;
}

void
TestCategoriesMocked::testCategoriesLimitOffset() {
    QFETCH(QList<CategoryDbRow>, rows);
    QFETCH(int, limit);
    QFETCH(int, offset);

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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ALL_CATEGORIES_TYPE_LIMIT)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":type"),
                                        Matcher<const QVariant&>(_),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":limit"),
                                        Matcher<const QVariant&>(QVariant(limit)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    EXPECT_CALL(*query.get(), bindValue(Matcher<const QString&>(":offset"),
                                        Matcher<const QVariant&>(QVariant(offset)),
                                        Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

    Sequence sequence;
    foreach(const CategoryDbRow& row, rows) {
        EXPECT_CALL(*query.get(), value(0))
                .InSequence(sequence)
                .WillOnce(Return(row.uuid));

        EXPECT_CALL(*query.get(), value(1))
                .InSequence(sequence)
                .WillOnce(Return(row.parent));

        EXPECT_CALL(*query.get(), value(2))
                .InSequence(sequence)
                .WillOnce(Return(row.name));

        EXPECT_CALL(*query.get(), value(3))
                .InSequence(sequence)
                .WillOnce(Return(row.type));

        EXPECT_CALL(*query.get(), value(4))
                .InSequence(sequence)
                .WillOnce(Return(row.color));
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
    auto cats = book.categories(chancho::Category::Type::INCOME, limit, offset);

    QVERIFY(!book.isError());
    QCOMPARE(cats.count(), rows.count());

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testCategoriesExecError() {
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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_ALL_CATEGORIES)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillRepeatedly(Return(error));

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillRepeatedly(Return(false));

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto cats = book.categories();

    QVERIFY(book.isError());
    QCOMPARE(cats.count(), 0);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testNumberOfCategoriesNoType_data() {
    QTest::addColumn<int>("dbCount");

    QTest::newRow("income-cat") << 4;
    QTest::newRow("expense-cat") << 5;
}

void
TestCategoriesMocked::testNumberOfCategoriesNoType() {
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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_CATEGORIES_COUNT)))
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
    auto count = book.numberOfCategories();

    QVERIFY(!book.isError());
    QCOMPARE(dbCount, count);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testNumberOfCategoriesType_data() {
    QTest::addColumn<chancho::Category::Type>("type");
    QTest::addColumn<int>("dbCount");

    QTest::newRow("income-cat") << chancho::Category::Type::INCOME << 4;
    QTest::newRow("expense-cat") << chancho::Category::Type::EXPENSE << 8;
}

void
TestCategoriesMocked::testNumberOfCategoriesType() {
    QFETCH(chancho::Category::Type, type);
    QFETCH(int, dbCount);

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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_CATEGORIES_COUNT_TYPE)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*query.get(),
                bindValue(Matcher<const QString&>(":type"),
                          Matcher<const QVariant&>(static_cast<int>(type)),
                          Matcher<QFlags<QSql::ParamTypeFlag>>(_)))
            .Times(1);

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
    auto count = book.numberOfCategories(type);

    QVERIFY(!book.isError());
    QCOMPARE(dbCount, count);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

void
TestCategoriesMocked::testNumberOfCategoriesExecError() {
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

    EXPECT_CALL(*query.get(), prepare(QStringEqual(SELECT_CATEGORIES_COUNT)))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*db.get(), lastError())
            .Times(0);

    EXPECT_CALL(*query.get(), exec())
            .Times(1)
            .WillRepeatedly(Return(false));

    EXPECT_CALL(*db.get(), lastError())
            .Times(1)
            .WillOnce(Return(error));

    EXPECT_CALL(*query.get(), next())
            .Times(0);

    EXPECT_CALL(*db.get(), close())
            .Times(1);

    PublicBook book;
    auto count = book.numberOfCategories();

    QVERIFY(book.isError());
    QCOMPARE(-1, count);

    // verify expectations
    QVERIFY(Mock::VerifyAndClearExpectations(_dbFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(db.get()));
    QVERIFY(Mock::VerifyAndClearExpectations(query.get()));
}

}
}
}

QTEST_MAIN(com::chancho::tests::TestCategoriesMocked)

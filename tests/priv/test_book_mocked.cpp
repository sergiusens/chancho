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
#include <QStandardPaths>

#include "matchers.h"
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
    QVERIFY(QFileInfo::exists(fi.path()));
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
        .Times(2)
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
        .Times(2)
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

QTEST_MAIN(TestBookMocked)

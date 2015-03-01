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

#include <QSignalSpy>
#include "test_day_model.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;

void
TestDayModel::init() {
    BaseTestCase::init();
}

void
TestDayModel::cleanup() {
    BaseTestCase::cleanup();
}

void
TestDayModel::testRowCountInvalidModel() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto noDayModel = std::make_shared<com::chancho::tests::PublicDayModel>(book);

    EXPECT_CALL(*book.get(), numberOfTransactions(_, _, _))
            .Times(0);

    noDayModel->rowCount(QModelIndex());
    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestDayModel::testRowCount() {
    int count = 5;
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), numberOfTransactions(day, month, year))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto result = model->rowCount(QModelIndex());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
    QCOMPARE(result, count);
}

void
TestDayModel::testRowCountError() {
    int count = 5;
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), numberOfTransactions(day, month, year))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto result = model->rowCount(QModelIndex());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
    QCOMPARE(result, 0);  // not equal to count because we had an error
}

void
TestDayModel::testDataNotValidIndex() {
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    auto result = model->data(QModelIndex(), Qt::DisplayRole);

    QVERIFY(!result.isValid());
    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestDayModel::testDataOutOfIndex() {
    int count = 5;
    int index = count + 1;
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), numberOfTransactions(day, month, year))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto result = model->data(index, Qt::DisplayRole);
    QVERIFY(!result.isValid());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestDayModel::testDataBookError() {
    int count = 5;
    int index = count - 1;
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), numberOfTransactions(day, month, year))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), transactions(day, month, year, 1, index))
            .Times(1)
            .WillOnce(Return(QList<com::chancho::TransactionPtr>()));

    EXPECT_CALL(*book.get(), isError())
            .Times(2)
            .WillOnce(Return(false))
            .WillOnce(Return(true));

    EXPECT_CALL(*book.get(), lastError())
            .Times(1)
            .WillOnce(Return(QString("Foo")));

    auto result = model->data(index, Qt::DisplayRole);
    QVERIFY(!result.isValid());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestDayModel::testDataNoData() {
    int count = 5;
    int index = count - 1;
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), numberOfTransactions(day, month, year))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), transactions(day, month, year, 1, index))
            .Times(1)
            .WillOnce(Return(QList<com::chancho::TransactionPtr>()));

    EXPECT_CALL(*book.get(), isError())
            .Times(2)
            .WillOnce(Return(false))
            .WillOnce(Return(false));

    auto result = model->data(index, Qt::DisplayRole);
    QVERIFY(!result.isValid());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestDayModel::testDataGetTransaction() {
    int count = 5;
    int index = count - 1;
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    auto transaction = std::make_shared<com::chancho::Transaction>();
    QList<com::chancho::TransactionPtr> list;
    list.append(transaction);


    EXPECT_CALL(*book.get(), numberOfTransactions(day, month, year))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), transactions(day, month, year, 1, index))
            .Times(1)
            .WillOnce(Return(list));

    EXPECT_CALL(*book.get(), isError())
            .Times(2)
            .WillOnce(Return(false))
            .WillOnce(Return(false));

    auto result = model->data(index, Qt::DisplayRole);
    QVERIFY(result.isValid());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestDayModel::testGetDay() {
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getDay(), day);
}

void
TestDayModel::testSetDayNoSignal() {
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(dayChanged(int)));
    model->setDay(day);
    QCOMPARE(spy.count(), 0);
}

void
TestDayModel::testSetDaySignal() {
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(dayChanged(int)));
    model->setDay(day + 1);
    QCOMPARE(spy.count(), 1);
}

void
TestDayModel::testGetMonth() {
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getMonth(), month);
}

void
TestDayModel::testSetMonthNoSignal() {
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(monthChanged(int)));
    model->setMonth(month);
    QCOMPARE(spy.count(), 0);
}

void
TestDayModel::testSetMonthSignal() {
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(monthChanged(int)));
    model->setMonth(month + 1);
    QCOMPARE(spy.count(), 1);
}

void
TestDayModel::testGetYear() {
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getYear(), year);
}

void
TestDayModel::testSetYearNoSignal() {
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(yearChanged(int)));
    model->setYear(year);
    QCOMPARE(spy.count(), 0);
}

void
TestDayModel::testSetYearSignal() {
    int day = 1;
    int month = 3;
    int year = 4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(yearChanged(int)));
    model->setYear(year + 1);
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(TestDayModel)

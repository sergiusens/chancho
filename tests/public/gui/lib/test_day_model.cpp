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
    int year = 2014;

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
    int year = 2014;

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
    int year = 2014;

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
    int year = 2014;

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
    int year = 2014;

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
    int year = 2014;

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
    int year = 2014;

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
    int year = 2014;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getDay(), day);
}

void
TestDayModel::testSetDayNoSignal() {
    int day = 1;
    int month = 3;
    int year = 2014;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), expenseForDay(day, month, year))
            .Times(0);

    EXPECT_CALL(*book.get(), incomeForDay(day, month, year))
            .Times(0);

    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(dayChanged(int)));
    QSignalSpy dateSpy(model.get(), SIGNAL(dateChanged(QDate)));
    QSignalSpy nameSpy(model.get(), SIGNAL(dayNameChanged(QString)));
    model->setDay(day);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(dateSpy.count(), 0);
    QCOMPARE(nameSpy.count(), 0);
}

void
TestDayModel::testSetDaySignal() {
    int day = 1;
    int newDay = day + 1;
    int month = 3;
    int year = 2014;
    double expense = 2.3;
    double income = 20.3;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), expenseForDay(newDay, month, year))
            .Times(1)
            .WillOnce(Return(expense));

    EXPECT_CALL(*book.get(), incomeForDay(newDay, month, year))
            .Times(1)
            .WillOnce(Return(income));

    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(dayChanged(int)));
    QSignalSpy dateSpy(model.get(), SIGNAL(dateChanged(QDate)));
    QSignalSpy nameSpy(model.get(), SIGNAL(dayNameChanged(QString)));
    model->setDay(newDay);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(dateSpy.count(), 1);
    QCOMPARE(nameSpy.count(), 1);
}

void
TestDayModel::testGetMonth() {
    int day = 1;
    int month = 3;
    int year = 2014;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getMonth(), month);
}

void
TestDayModel::testSetMonthNoSignal() {
    int day = 1;
    int month = 3;
    int year = 2014;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), expenseForDay(day, month, year))
            .Times(0);

    EXPECT_CALL(*book.get(), incomeForDay(day, month, year))
            .Times(0);

    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(monthChanged(int)));
    QSignalSpy dateSpy(model.get(), SIGNAL(dateChanged(QDate)));
    model->setMonth(month);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(dateSpy.count(), 0);
}

void
TestDayModel::testSetMonthSignal() {
    int day = 1;
    int month = 3;
    int newMonth = month + 1;
    int year = 2014;
    double income = 30.4;
    double expense = 23.0;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), expenseForDay(day, newMonth, year))
            .Times(1)
            .WillOnce(Return(expense));

    EXPECT_CALL(*book.get(), incomeForDay(day, newMonth, year))
            .Times(1)
            .WillOnce(Return(income));

    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(monthChanged(int)));
    QSignalSpy dateSpy(model.get(), SIGNAL(dateChanged(QDate)));
    model->setMonth(newMonth);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(dateSpy.count(), 1);
}

void
TestDayModel::testGetYear() {
    int day = 1;
    int month = 3;
    int year = 2014;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);
    QCOMPARE(model->getYear(), year);
}

void
TestDayModel::testSetYearNoSignal() {
    int day = 1;
    int month = 3;
    int year = 2014;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), expenseForDay(day, month, year))
            .Times(0);

    EXPECT_CALL(*book.get(), incomeForDay(day, month, year))
            .Times(0);

    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(yearChanged(int)));
    QSignalSpy dateSpy(model.get(), SIGNAL(dateChanged(QDate)));
    model->setYear(year);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(dateSpy.count(), 0);
}

void
TestDayModel::testSetYearSignal() {
    int day = 1;
    int month = 3;
    int year = 2014;
    int newYear = year + 1;
    double income = 3.4;
    double expense = 4.5;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(day, month, year, book);

    EXPECT_CALL(*book.get(), expenseForDay(day, month, newYear))
            .Times(1)
            .WillOnce(Return(expense));

    EXPECT_CALL(*book.get(), incomeForDay(day, month, newYear))
            .Times(1)
            .WillOnce(Return(income));

    QCOMPARE(model->getMonth(), month);

    QSignalSpy spy(model.get(), SIGNAL(yearChanged(int)));
    QSignalSpy dateSpy(model.get(), SIGNAL(dateChanged(QDate)));
    model->setYear(newYear);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(dateSpy.count(), 1);
}

void
TestDayModel::testSetDate_data() {
    QTest::addColumn<QDate>("oldDate");
    QTest::addColumn<QDate>("newDate");
    QTest::addColumn<bool>("daySignal");
    QTest::addColumn<bool>("monthSignal");
    QTest::addColumn<bool>("yearSignal");
    QTest::addColumn<double >("expense");
    QTest::addColumn<double >("income");

    QDate dayDate(2014, 1, 1);
    QDate newDayDate(2014, 1, 2);

    QTest::newRow("day-changed") << dayDate << newDayDate << true << false << false << 30.0 << 90.3;

    QDate monthDate(2014, 1, 1);
    QDate newMonthDate(2014, 2, 1);

    QTest::newRow("month-changed") << monthDate << newMonthDate << false << true << false << 10.0 << 190.3;

    QDate yearDate(2014, 1, 1);
    QDate newYearDate(2015, 1, 1);

    QTest::newRow("year-changed") << yearDate << newYearDate << false << false << true << 110.20 << 80.3;

    QDate dayMonthDate(2014, 1, 1);
    QDate newDayMonthDate(2014, 2, 2);

    QTest::newRow("day-month-changed") << dayMonthDate << newDayMonthDate << true << true << false << 112.20 << 280.3;

    QDate dayMonthYearDate(2014, 1, 1);
    QDate newDayMonthYearDate(2015, 2, 2);

    QTest::newRow("day-month-year-changed") << dayMonthYearDate << newDayMonthYearDate << true << true << true
        << 1129.20 << 1280.3;
}

void
TestDayModel::testSetDate() {
    QFETCH(QDate, oldDate);
    QFETCH(QDate, newDate);
    QFETCH(bool, daySignal);
    QFETCH(bool, monthSignal);
    QFETCH(bool, yearSignal);
    QFETCH(double, expense);
    QFETCH(double, income);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(oldDate, book);

    EXPECT_CALL(*book.get(), expenseForDay(newDate.day(), newDate.month(), newDate.year()))
            .Times(1)
            .WillOnce(Return(expense));

    EXPECT_CALL(*book.get(), incomeForDay(newDate.day(), newDate.month(), newDate.year()))
            .Times(1)
            .WillOnce(Return(income));

    QSignalSpy daySpy(model.get(), SIGNAL(dayChanged(int)));
    QSignalSpy monthSpy(model.get(), SIGNAL(monthChanged(int)));
    QSignalSpy yearSpy(model.get(), SIGNAL(yearChanged(int)));
    QSignalSpy dateSpy(model.get(), SIGNAL(dateChanged(QDate)));

    model->setDate(newDate);

    if (daySignal) {
        QCOMPARE(daySpy.count(), 1);
    } else {
        QCOMPARE(daySpy.count(), 0);
    }

    if (monthSignal) {
        QCOMPARE(monthSpy.count(), 1);
    } else {
        QCOMPARE(monthSpy.count(), 0);
    }

    if (yearSignal) {
        QCOMPARE(yearSpy.count(), 1);
    } else {
        QCOMPARE(yearSpy.count(), 0);
    }

    QCOMPARE(dateSpy.count(), 1);

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestDayModel::testGetIncome() {
    QDate date(2014, 2, 1);
    double income = 34.0;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(date, book);

    EXPECT_CALL(*book.get(), incomeForDay(date.day(), date.month(), date.year()))
            .Times(1)
            .WillOnce(Return(income));

    auto result = model->getIncomeSum();

    QCOMPARE(result, income);
    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestDayModel::testGetExpense() {
    QDate date(2014, 2, 1);
    double expense = 34.0;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(date, book);

    EXPECT_CALL(*book.get(), expenseForDay(date.day(), date.month(), date.year()))
            .Times(1)
            .WillOnce(Return(expense));

    auto result = model->getExpenseSum();

    QCOMPARE(result, expense);
    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestDayModel::testGetDate() {
    QDate date(2014, 2, 1);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(date, book);
    QCOMPARE(date, model->getDate());
}

void
TestDayModel::testGetDateName() {
    QDate date(2014, 2, 1);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicDayModel>(date, book);
    QCOMPARE(model->getDayName(), QDate::shortDayName(date.dayOfWeek()));
}

QTEST_MAIN(TestDayModel)

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

#include <com/chancho/qml/recurrent_transaction.h>
#include "public_account.h"
#include "public_qml_recurrent_transaction.h"
#include "test_recurrent_transaction.h"

void
TestRecurrentTransaction::init() {
    BaseTestCase::init();
}

void
TestRecurrentTransaction::cleanup() {
    BaseTestCase::cleanup();
}

void
TestRecurrentTransaction::testGetAccount_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<double>("initialAmount");
    QTest::addColumn<double>("amount");
    QTest::addColumn<QString>("memo");
    QTest::addColumn<QString>("color");

    QTest::newRow("bankia-obj") << "Bankia" << 50.0 << 3004.89 << "Savings account" << "#ff";
    QTest::newRow("bbva-obj") << "BBVA" << 10.9 << 23.9 << "Student loan" << "#234523";
}

void
TestRecurrentTransaction::testGetAccount() {
    QFETCH(QString, name);
    QFETCH(double, initialAmount);
    QFETCH(double, amount);
    QFETCH(QString, memo);
    QFETCH(QString, color);

    // create the category and store it
    auto account = std::make_shared<PublicAccount>(name, amount, memo, color);
    account->initialAmount = initialAmount;
    auto category = std::make_shared<com::chancho::Category>("Test category", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, .50, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>());

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(account->name, qmlTransaction->getAccount());
}

void
TestRecurrentTransaction::testGetAmount_data() {
    QTest::addColumn<double>("amount");

    QTest::newRow("first-amount") << 0.5;
    QTest::newRow("second-amount") << 130.45;
}

void
TestRecurrentTransaction::testGetAmount() {
    QFETCH(double, amount);

    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Test category", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>());

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(amount, qmlTransaction->getAmount());
}

void
TestRecurrentTransaction::testSetAmountNoSignal() {
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Test category", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
                                                                             std::make_shared<com::chancho::RecurrentTransaction::Recurrence>());

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);

    QSignalSpy spy(qmlTransaction.get(), SIGNAL(amountChanged(double)));
    qmlTransaction->setAmount(amount);
    QCOMPARE(spy.count(), 0);
}

void
TestRecurrentTransaction::testSetAmount() {
    auto amount = .45;
    auto newAmount = 2.45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Test category", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>());

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);

    QSignalSpy spy(qmlTransaction.get(), SIGNAL(amountChanged(double)));
    qmlTransaction->setAmount(newAmount);
    QCOMPARE(spy.count(), 1);
}

void
TestRecurrentTransaction::testGetCategory_data() {
    QTest::addColumn<QString>("categoryName");

    QTest::newRow("first-category") << "My testing cat";
    QTest::newRow("second-category") << "Sushi: Food";
}

void
TestRecurrentTransaction::testGetCategory() {
    QFETCH(QString, categoryName);

    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>(categoryName, com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>());

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(categoryName, qmlTransaction->getCategory());
}

void
TestRecurrentTransaction::testGetStartDate_data() {
    QTest::addColumn<QDate>("startDate");

    QTest::newRow("first-date") << QDate::currentDate().addDays(2);
    QTest::newRow("second-date") << QDate::currentDate().addYears(2);
}

void
TestRecurrentTransaction::testGetStartDate() {
    QFETCH(QDate, startDate);

    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(
                com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, startDate));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(qmlTransaction->getStartDate(), startDate);
}

void
TestRecurrentTransaction::testGetEndDate_data() {
    QTest::addColumn<QDate>("endDate");

    QTest::newRow("first-date") << QDate::currentDate().addDays(2);
    QTest::newRow("second-date") << QDate::currentDate().addYears(2);
}

void
TestRecurrentTransaction::testGetEndDate() {
    QFETCH(QDate, endDate);

    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(
                com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate(), endDate));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(qmlTransaction->getEndDate(), endDate);
}

void
TestRecurrentTransaction::testSetEndDateNoSignal() {
    auto endDate = QDate::currentDate().addYears(1);
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(
                com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate(), endDate));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(endDateChanged(QDate)));
    qmlTransaction->setEndDate(endDate);
    QCOMPARE(spy.count(), 0);
}

void
TestRecurrentTransaction::testSetEndDate() {
    auto endDate = QDate::currentDate().addYears(1);
    auto newEndDate = endDate.addDays(1);
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(
                com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate(), endDate));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(endDateChanged(QDate)));
    qmlTransaction->setEndDate(newEndDate);
    QCOMPARE(spy.count(), 1);
}

void
TestRecurrentTransaction::testGetOccurrences_data() {
    QTest::addColumn<int>("occurrences");

    QTest::newRow("first-occurrenes") << 10;
    QTest::newRow("second-occurrences") << 100;
}

void
TestRecurrentTransaction::testGetOccurrences() {
    QFETCH(int, occurrences);
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(
                com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate(), occurrences));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(qmlTransaction->getOccurrences(), occurrences);
}

void
TestRecurrentTransaction::testSetOccurrencesNoSignal() {
    auto occurrences = 10;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(
                com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate(), occurrences));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(occurrencesChanged(int)));
    qmlTransaction->setOccurrences(occurrences);
    QCOMPARE(spy.count(), 0);
}

void
TestRecurrentTransaction::testSetOccurrences() {
    auto occurrences = 10;
    auto newOccurrences = occurrences + 10;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(
                com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, QDate::currentDate(), occurrences));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(occurrencesChanged(int)));
    qmlTransaction->setOccurrences(newOccurrences);
    QCOMPARE(spy.count(), 1);
}

void
TestRecurrentTransaction::testGetNumberOfDays_data() {
    QTest::addColumn<int>("numberOfDays");

    QTest::newRow("first-days") << 10;
    QTest::newRow("second-days") << 100;
}

void
TestRecurrentTransaction::testGetNumberOfDays() {
    QFETCH(int, numberOfDays);

    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(numberOfDays, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(qmlTransaction->getNumberOfDays(), numberOfDays);
}

void
TestRecurrentTransaction::testSetNumberOfDaysNoSignal() {
    auto numberOfDays = 3;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(numberOfDays, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(numberOfDaysChanged(int)));
    qmlTransaction->setNumberOfDays(numberOfDays);
    QCOMPARE(spy.count(), 0);
}

void
TestRecurrentTransaction::testSetNumberOfDays() {
    auto numberOfDays = 3;
    auto newNumberOfDays = numberOfDays + 3;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(numberOfDays, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(numberOfDaysChanged(int)));
    qmlTransaction->setNumberOfDays(newNumberOfDays);
    QCOMPARE(spy.count(), 1);
}

void
TestRecurrentTransaction::testGetContents() {
    auto contents = QString("Dinner with friends");
    auto numberOfDays = 3;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->contents = contents;
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(numberOfDays, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(qmlTransaction->getContents(), contents);
}

void
TestRecurrentTransaction::testSetContentsNoSignal() {
    auto contents = QString("Dinner with friends");
    auto numberOfDays = 3;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->contents = contents;
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(numberOfDays, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(contentsChanged(QString)));
    qmlTransaction->setContents(contents);
    QCOMPARE(spy.count(), 0);
}

void
TestRecurrentTransaction::testSetContents() {
    auto contents = QString("Dinner with friends");
    auto newContents = contents + "23";
    auto numberOfDays = 3;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->contents = contents;
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(numberOfDays, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(contentsChanged(QString)));
    qmlTransaction->setContents(newContents);
    QCOMPARE(spy.count(), 1);
}

void
TestRecurrentTransaction::testGetMemo() {
    auto memo = QString("Dinner with friends");
    auto numberOfDays = 3;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->memo = memo;
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(numberOfDays, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(qmlTransaction->getMemo(), memo);
}

void
TestRecurrentTransaction::testSetMemoNoSignal() {
    auto memo = QString("Dinner with friends");
    auto numberOfDays = 3;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->memo = memo;
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(numberOfDays, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(memoChanged(QString)));
    qmlTransaction->setMemo(memo);
    QCOMPARE(spy.count(), 0);
}

void
TestRecurrentTransaction::testSetMemo() {
    auto memo = QString("Dinner with friends");
    auto newMemo = memo + "23";
    auto numberOfDays = 3;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->memo = memo;
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(numberOfDays, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(memoChanged(QString)));
    qmlTransaction->setMemo(newMemo);
    QCOMPARE(spy.count(), 1);
}

void
TestRecurrentTransaction::testGetType_data() {
    QTest::addColumn<com::chancho::Category::Type>("transactionType");
    QTest::addColumn<com::chancho::qml::Book::TransactionType>("qmlType");

    QTest::newRow("income") << com::chancho::Category::Type::INCOME << com::chancho::qml::Book::INCOME;
    QTest::newRow("expense") << com::chancho::Category::Type::EXPENSE << com::chancho::qml::Book::EXPENSE;
}

void
TestRecurrentTransaction::testGetType() {
    QFETCH(com::chancho::Category::Type, transactionType);
    QFETCH(com::chancho::qml::Book::TransactionType, qmlType);

    auto numberOfDays = 3;
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", transactionType);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(numberOfDays, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(qmlTransaction->getType(), qmlType);
}

void
TestRecurrentTransaction::testGetRecurrenceType_data() {
    QTest::addColumn<com::chancho::RecurrentTransaction::Recurrence::Defaults>("recurrenceType");
    QTest::addColumn<com::chancho::qml::Book::RecurrenceType>("qmlType");

    QTest::newRow("daily") << com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY
        << com::chancho::qml::Book::DAILY;
    QTest::newRow("weekly") << com::chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY
        << com::chancho::qml::Book::WEEKLY;
    QTest::newRow("monthly") << com::chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY
        << com::chancho::qml::Book::MONTHLY;
}

void
TestRecurrentTransaction::testGetRecurrenceType() {
    QFETCH(com::chancho::RecurrentTransaction::Recurrence::Defaults, recurrenceType);
    QFETCH(com::chancho::qml::Book::RecurrenceType, qmlType);

    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    auto recurrentPtr = std::make_shared<com::chancho::RecurrentTransaction>(transactionPtr,
        std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(recurrenceType, QDate::currentDate()));

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrentPtr);
    QCOMPARE(qmlTransaction->getRecurrenceType(), qmlType);
}

QTEST_MAIN(TestRecurrentTransaction)

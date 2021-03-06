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
#include <com/chancho/qml/workers/transactions/single_store.h>
#include <com/chancho/qml/book.h>

#include "book.h"
#include "matchers.h"

#include "test_single_store.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::Return;

namespace c = com::chancho;

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace transactions {

namespace tests {

namespace {
    const QString RECURRENCE_TYPE_KEY = "type";
    const QString RECURRENCE_DAYS_KEY = "days";
    const QString END_KEY = "end";
    const QString END_DATE_KEY = "date";
    const QString END_OCCURRENCES_KEY = "ocurrences";
}

void
TestSingleStore::init() {
    BaseTestCase::init();
}

void
TestSingleStore::cleanup() {
    BaseTestCase::cleanup();
}

void
TestSingleStore::testRun_data() {
    QTest::addColumn<QString>("accountName");
    QTest::addColumn<double>("amount");
    QTest::addColumn<QString>("categoryName");
    QTest::addColumn<QString>("content");
    QTest::addColumn<QString>("memo");

    QTest::newRow("first-obj") << QUuid::createUuid().toString() << 20.0 << QUuid::createUuid().toString()
    << "My content" << "My memo";
    QTest::newRow("second-obj") << QUuid::createUuid().toString() << 20.0 << QUuid::createUuid().toString()
    << "My content" << "My memo";
    QTest::newRow("last-obj") << QUuid::createUuid().toString() << 20.0 << QUuid::createUuid().toString()
    << "My content" << "My memo";
}

void
TestSingleStore::testRun() {
    QFETCH(QString, accountName);
    QFETCH(double, amount);
    QFETCH(QString, categoryName);
    QFETCH(QString, content);
    QFETCH(QString, memo);

    auto acc = std::make_shared<c::Account>(accountName);
    auto cat = std::make_shared<c::Category>(categoryName, c::Category::Type::INCOME);
    auto tran = std::make_shared<c::Transaction>(acc, amount, cat, content, memo);

    auto book = std::make_shared<c::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                store(Matcher<c::TransactionPtr>(TransactionEquals(tran))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto worker = std::make_shared<c::qml::workers::transactions::SingleStore>(book, tran->account,
        tran->category, tran->date, tran->amount, tran->contents, tran->memo);

    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failureSpy.count(), 0);
}

void
TestSingleStore::testRunBookError() {
    auto acc = std::make_shared<c::Account>("Bankia");
    auto cat = std::make_shared<c::Category>("Food", c::Category::Type::INCOME);
    auto tran = std::make_shared<c::Transaction>(acc, 90.0, cat, "", "");
    auto book = std::make_shared<c::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                store(Matcher<c::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto worker = std::make_shared<c::qml::workers::transactions::SingleStore>(book, tran->account,
        tran->category, tran->date, tran->amount, tran->contents, tran->memo);
    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failureSpy.count(), 1);
}

void
TestSingleStore::testRecurrenceStoreTypeMissing() {
    // create a badly formatter recurrene to ensure that we do fire a failure signal
    QVariantMap recurrence;
    recurrence["test"] = "Not useful data";

    auto acc = std::make_shared<c::Account>("Bankia");
    auto cat = std::make_shared<c::Category>("Food", c::Category::Type::INCOME);
    auto tran = std::make_shared<c::Transaction>(acc, 90.0, cat, "", "");
    auto book = std::make_shared<c::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                store(Matcher<c::RecurrentTransactionPtr>(_), false))
            .Times(0);

    auto worker = std::make_shared<c::qml::workers::transactions::SingleStore>(book, tran->account,
        tran->category, tran->date, tran->amount, tran->contents, tran->memo, recurrence);
    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failureSpy.count(), 1);
}

void
TestSingleStore::testRecurrenceStoreBothEndPresent() {
    // create a badly formatter recurrene to ensure that we do fire a failure signal
    QVariantMap recurrence;
    recurrence[RECURRENCE_DAYS_KEY] = 4;
    QVariantMap endMap;
    endMap[END_DATE_KEY] = QDate::currentDate();
    endMap[END_OCCURRENCES_KEY] = 8;
    recurrence[END_KEY] = endMap;

    auto acc = std::make_shared<c::Account>("Bankia");
    auto cat = std::make_shared<c::Category>("Food", c::Category::Type::INCOME);
    auto tran = std::make_shared<c::Transaction>(acc, 90.0, cat, "", "");
    auto book = std::make_shared<c::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                store(Matcher<c::RecurrentTransactionPtr>(_), false))
            .Times(0);

    auto worker = std::make_shared<c::qml::workers::transactions::SingleStore>(book, tran->account,
        tran->category, tran->date, tran->amount, tran->contents, tran->memo, recurrence);
    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failureSpy.count(), 1);
}

void
TestSingleStore::testRecurrenctStore_data() {
    QTest::addColumn<c::TransactionPtr>("transaction");
    QTest::addColumn<c::RecurrentTransaction::RecurrencePtr>("recurrence");

    auto acc = std::make_shared<c::Account>("Bankia");
    auto cat = std::make_shared<c::Category>("Food", c::Category::Type::INCOME);
    auto tran = std::make_shared<c::Transaction>(acc, 90.0, cat, "", "");

    QTest::newRow("daily") << tran
        << std::make_shared<c::RecurrentTransaction::Recurrence>(
            c::RecurrentTransaction::Recurrence::Defaults::DAILY, tran->date);
    QTest::newRow("weekly") << tran
        << std::make_shared<c::RecurrentTransaction::Recurrence>(
            c::RecurrentTransaction::Recurrence::Defaults::WEEKLY, tran->date);
    QTest::newRow("monthly") << tran
        << std::make_shared<c::RecurrentTransaction::Recurrence>(
            c::RecurrentTransaction::Recurrence::Defaults::MONTHLY, tran->date);
    QTest::newRow("other") << tran << std::make_shared<c::RecurrentTransaction::Recurrence>(4, tran->date);
    QTest::newRow("daily-end-date") << tran
        << std::make_shared<c::RecurrentTransaction::Recurrence>(
            c::RecurrentTransaction::Recurrence::Defaults::DAILY, tran->date, tran->date.addYears(1));
    QTest::newRow("weekly-end-date") << tran
        << std::make_shared<c::RecurrentTransaction::Recurrence>(
            c::RecurrentTransaction::Recurrence::Defaults::WEEKLY, tran->date, tran->date.addYears(1));
    QTest::newRow("monthly-end-date") << tran
        << std::make_shared<c::RecurrentTransaction::Recurrence>(
            c::RecurrentTransaction::Recurrence::Defaults::MONTHLY, tran->date, tran->date.addYears(1));
    QTest::newRow("other-end-date") << tran
        << std::make_shared<c::RecurrentTransaction::Recurrence>(4, tran->date, tran->date.addYears(1));
    QTest::newRow("daily-occurrences") << tran
        << std::make_shared<c::RecurrentTransaction::Recurrence>(
            c::RecurrentTransaction::Recurrence::Defaults::DAILY, tran->date, 8);
    QTest::newRow("weekly-occurrences") << tran
        << std::make_shared<c::RecurrentTransaction::Recurrence>(
            c::RecurrentTransaction::Recurrence::Defaults::WEEKLY, tran->date, 8);
    QTest::newRow("monthly-occurrences") << tran
        << std::make_shared<c::RecurrentTransaction::Recurrence>(
            c::RecurrentTransaction::Recurrence::Defaults::MONTHLY, tran->date, 8);
    QTest::newRow("other-occurrences") << tran << std::make_shared<c::RecurrentTransaction::Recurrence>(
            4, tran->date, 8);
}

void
TestSingleStore::testRecurrenctStore() {
    QFETCH(c::TransactionPtr, transaction);
    QFETCH(c::RecurrentTransaction::RecurrencePtr, recurrence);

    auto recurrentTransaction = std::make_shared<c::RecurrentTransaction>(transaction, recurrence);
    QVariantMap recurrenceMap;
    // create the map according to the recurrence
    if (recurrence->defaults()) {
        switch (recurrence->defaults().get()) {
            case c::RecurrentTransaction::Recurrence::Defaults::DAILY:
                recurrenceMap[RECURRENCE_TYPE_KEY] = qml::Book::DAILY;
                break;
            case c::RecurrentTransaction::Recurrence::Defaults::WEEKLY:
                recurrenceMap[RECURRENCE_TYPE_KEY] = qml::Book::WEEKLY;
                break;
            default:
                recurrenceMap[RECURRENCE_TYPE_KEY] = qml::Book::MONTHLY;
                break;
        }
    } else {
        recurrenceMap[RECURRENCE_DAYS_KEY] = recurrence->numberOfDays().get();
    }

    if (recurrence->endDate.isValid()) {
        QVariantMap endMap;
        endMap[END_DATE_KEY] = recurrence->endDate;
        recurrenceMap[END_KEY] = endMap;
    } else if (recurrence->occurrences) {
        QVariantMap endMap;
        endMap[END_OCCURRENCES_KEY] = recurrence->occurrences.get();
        recurrenceMap[END_KEY] = endMap;
    }

    auto book = std::make_shared<c::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                store(Matcher<c::RecurrentTransactionPtr>(RecurrentTransactionEquals(recurrentTransaction)), false))
            .Times(1);

    EXPECT_CALL(*book.get(), generateRecurrentTransactions())
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(2)
            .WillOnce(Return(false))
            .WillOnce(Return(false));

    auto worker = std::make_shared<c::qml::workers::transactions::SingleStore>(book, transaction->account,
        transaction->category, transaction->date, transaction->amount, transaction->contents, transaction->memo,
        recurrenceMap);
    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failureSpy.count(), 0);

}

}
}
}
}
}
}

QTEST_MAIN(com::chancho::qml::workers::transactions::tests::TestSingleStore)


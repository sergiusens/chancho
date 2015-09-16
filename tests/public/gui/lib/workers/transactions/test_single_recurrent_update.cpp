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
#include <com/chancho/account.h>
#include <com/chancho/qml/workers/transactions/single_recurrent_update.h>

#include "book.h"
#include "matchers.h"
#include "public_account.h"
#include "public_category.h"

#include "test_single_recurrent_update.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::Return;

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace transactions {

namespace tests {

void
TestSingleRecurrentUpdate::init() {
    BaseTestCase::init();
}

void
TestSingleRecurrentUpdate::cleanup() {
    BaseTestCase::cleanup();
}

void
TestSingleRecurrentUpdate::testRun_data() {
    QTest::addColumn<QPair<QString, QString>>("accountName");
    QTest::addColumn<QPair<double, double>>("amount");
    QTest::addColumn<QPair<QString, QString>>("categoryName");
    QTest::addColumn<QPair<QString, QString>>("content");
    QTest::addColumn<QPair<QString, QString>>("memo");
    QTest::addColumn<QPair<QDate, QDate>>("date");
    auto current = QDate::currentDate();

    QTest::newRow("diff-account") << QPair<QString, QString>("oldAccount", "newAccount") << QPair<double, double>(2, 2)
    << QPair<QString, QString>("category", "category") << QPair<QString, QString>("content", "content")
    << QPair<QString, QString>("memo", "memo") << QPair<QDate, QDate>(current, current);
    QTest::newRow("diff-amount") << QPair<QString, QString>("account", "account") << QPair<double, double>(2, 3)
    << QPair<QString, QString>("category", "category") << QPair<QString, QString>("content", "content")
    << QPair<QString, QString>("memo", "memo") << QPair<QDate, QDate>(current, current);
    QTest::newRow("diff-category") << QPair<QString, QString>("account", "account") << QPair<double, double>(2, 2)
    << QPair<QString, QString>("oldCategory", "newCategory") << QPair<QString, QString>("content", "content")
    << QPair<QString, QString>("memo", "memo") << QPair<QDate, QDate>(current, current);
    QTest::newRow("diff-content") << QPair<QString, QString>("account", "account") << QPair<double, double>(2, 2)
    << QPair<QString, QString>("category", "category") << QPair<QString, QString>("oldContent", "newContent")
    << QPair<QString, QString>("memo", "memo") << QPair<QDate, QDate>(current, current);
    QTest::newRow("diff-memo") << QPair<QString, QString>("account", "account") << QPair<double, double>(2, 2)
    << QPair<QString, QString>("category", "category") << QPair<QString, QString>("content", "content")
    << QPair<QString, QString>("oldMemo", "newMemo") << QPair<QDate, QDate>(current, current);
    QTest::newRow("diff-memo") << QPair<QString, QString>("account", "account") << QPair<double, double>(2, 2)
    << QPair<QString, QString>("category", "category") << QPair<QString, QString>("content", "content")
    << QPair<QString, QString>("memo", "memo") << QPair<QDate, QDate>(current, current.addDays(2));
}

void
TestSingleRecurrentUpdate::testRun() {
    typedef QPair<QString, QString> stringPair;
    typedef QPair<double , double> doublePair;
    typedef QPair<QDate, QDate> datePair;

    QFETCH(stringPair, accountName);
    QFETCH(doublePair, amount);
    QFETCH(stringPair, categoryName);
    QFETCH(stringPair, content);
    QFETCH(stringPair, memo);
    QFETCH(datePair, date);

    auto oldAcc = std::make_shared<PublicAccount>(accountName.first);
    oldAcc->_dbId = QUuid::createUuid();
    auto newAcc = std::make_shared<PublicAccount>(accountName.second);
    if (accountName.first != accountName.second) {
        newAcc->_dbId = QUuid::createUuid();
    } else {
        newAcc->_dbId = oldAcc->_dbId;
    }

    auto oldCat = std::make_shared<PublicCategory>(categoryName.first, com::chancho::Category::Type::INCOME);
    oldCat->_dbId = QUuid::createUuid();
    auto newCat = std::make_shared<PublicCategory>(categoryName.second, com::chancho::Category::Type::INCOME);
    if (categoryName.first != categoryName.second) {
        newCat->_dbId = QUuid::createUuid();
    } else {
        newCat->_dbId = oldCat->_dbId;
    }

    auto tran = std::make_shared<com::chancho::Transaction>(oldAcc, amount.first, oldCat, content.first, memo.first);
    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    recurrent->transaction = tran;
    recurrent->recurrence = std::make_shared<com::chancho::RecurrentTransaction::Recurrence>();
    recurrent->recurrence->startDate = date.first;
    auto expected = std::make_shared<com::chancho::Transaction>(newAcc, amount.second, newCat, content.second, memo.second);
    expected->date = date.second;
    auto expectedRecurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    expectedRecurrent->recurrence = std::make_shared<com::chancho::RecurrentTransaction::Recurrence>();
    expectedRecurrent->transaction = expected;

    auto book = std::make_shared<com::chancho::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::RecurrentTransactionPtr>(_), true))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto worker = std::make_shared<com::chancho::qml::workers::transactions::SingleRecurrentUpdate>(book, recurrent,
            newAcc, newCat, date.second, content.second, memo.second, amount.second, true);

    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failureSpy.count(), 0);
}

void
TestSingleRecurrentUpdate::testRunBookError() {
    auto acc = std::make_shared<com::chancho::Account>("Bankia");
    auto cat = std::make_shared<com::chancho::Category>("Food", com::chancho::Category::Type::INCOME);
    auto tran = std::make_shared<com::chancho::Transaction>(acc, 90.0, cat, "", "");
    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    recurrent->transaction = tran;
    auto book = std::make_shared<com::chancho::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::RecurrentTransactionPtr>(_), true))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto worker = std::make_shared<com::chancho::qml::workers::transactions::SingleRecurrentUpdate>(book, recurrent,
            tran->account, tran->category, recurrent->recurrence->startDate, tran->contents, tran->memo,
            tran->amount + 1, true);
    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failureSpy.count(), 1);
}

void
TestSingleRecurrentUpdate::testRunNoUpdate() {
    auto acc = std::make_shared<com::chancho::Account>("Bankia");
    auto cat = std::make_shared<com::chancho::Category>("Food", com::chancho::Category::Type::INCOME);
    auto tran = std::make_shared<com::chancho::Transaction>(acc, 90.0, cat, "", "");
    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    recurrent->transaction = tran;
    auto book = std::make_shared<com::chancho::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::RecurrentTransactionPtr>(_), true))
            .Times(0);

    EXPECT_CALL(*book.get(), isError())
            .Times(0);

    auto worker = std::make_shared<com::chancho::qml::workers::transactions::SingleRecurrentUpdate>(book, recurrent,
        tran->account, tran->category, recurrent->recurrence->startDate, tran->contents, tran->memo, tran->amount, true);
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

QTEST_MAIN(com::chancho::qml::workers::transactions::tests::TestSingleRecurrentUpdate)


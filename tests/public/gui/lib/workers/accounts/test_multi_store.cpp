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

#include <QVariantList>
#include <QSignalSpy>

#include <com/chancho/account.h>
#include <com/chancho/qml/workers/accounts/multi_store.h>

#include "book.h"
#include "matchers.h"
#include "test_multi_store.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::Return;

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace accounts {

namespace tests {

namespace {
    const QString NAME_KEY = "name";
    const QString MEMO_KEY = "memo";
    const QString COLOR_KEY = "color";
    const QString AMOUNT_KEY = "amount";
}

void
TestMultiStore::init() {
    BaseTestCase::init();
}

void
TestMultiStore::cleanup() {
    BaseTestCase::cleanup();
}

void
TestMultiStore::testRun_data() {
    QTest::addColumn<QVariantList>("accounts");
    QTest::addColumn<QList<com::chancho::AccountPtr>>("expected");

    QVariantList empty;
    QList<com::chancho::AccountPtr> emptyExpected;

    QTest::newRow("empty-list") << empty << emptyExpected;

    QList<com::chancho::AccountPtr> singleExpected;
    singleExpected.append(std::make_shared<com::chancho::Account>(QUuid::createUuid().toString(),
        90, "Random Memo", "#342"));
    singleExpected.at(0)->initialAmount = singleExpected.at(0)->amount;

    QVariantMap singleMap;
    singleMap[NAME_KEY] = singleExpected.at(0)->name;
    singleMap[MEMO_KEY] = singleExpected.at(0)->memo;
    singleMap[COLOR_KEY] = singleExpected.at(0)->color;
    singleMap[AMOUNT_KEY] = singleExpected.at(0)->amount;

    QVariantList singleAccount;
    singleAccount.append(singleMap);

    QTest::newRow("single-account-list") << singleAccount << singleExpected;

    QVariantList multiAccount;
    QList<com::chancho::AccountPtr> multiExpected;

    for(int index = 0; index < 10; index++) {
        auto acc = std::make_shared<com::chancho::Account>(QUuid::createUuid().toString(),
            index, QString("Memo for %1").arg(index), QString("Color for %1").arg(index));
        acc->initialAmount = index;
        QVariantMap map;
        map[NAME_KEY] = acc->name;
        map[MEMO_KEY] = acc->memo;
        map[COLOR_KEY] = acc->color;
        map[AMOUNT_KEY] = acc->amount;
        multiAccount.append(map);
        multiExpected.append(acc);

    }

    QTest::newRow("multi-account-list") << multiAccount << multiExpected;
}

void
TestMultiStore::testRun() {
    QFETCH(QVariantList, accounts);
    QFETCH(QList<com::chancho::AccountPtr>, expected);
    auto book = std::make_shared<com::chancho::tests::MockBook>();

    // set the expectations
    EXPECT_CALL(*book.get(),
                store(Matcher<QList<com::chancho::AccountPtr>>(AccountListEquals(expected))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto worker = std::make_shared<com::chancho::qml::workers::accounts::MultiStore>(book, accounts);
    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failureSpy.count(), 0);
}

void
TestMultiStore::testRunBookError() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    QVariantList accounts;

    EXPECT_CALL(*book.get(),
                store(Matcher<QList<com::chancho::AccountPtr>>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto worker = std::make_shared<com::chancho::qml::workers::accounts::MultiStore>(book, accounts);
    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failureSpy.count(), 1);
}

}
}
}
}
}
}

QTEST_MAIN(com::chancho::qml::workers::accounts::tests::TestMultiStore)


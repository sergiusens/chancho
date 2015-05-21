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

#include "book.h"
#include "matchers.h"

#include "test_single_store.h"

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

    auto acc = std::make_shared<com::chancho::Account>(accountName);
    auto cat = std::make_shared<com::chancho::Category>(categoryName, com::chancho::Category::Type::INCOME);
    auto tran = std::make_shared<com::chancho::Transaction>(acc, amount, cat, content, memo);

    auto book = std::make_shared<com::chancho::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::TransactionPtr>(TransactionEquals(tran))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto worker = std::make_shared<com::chancho::qml::workers::transactions::SingleStore>(book, tran->account,
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
    auto acc = std::make_shared<com::chancho::Account>("Bankia");
    auto cat = std::make_shared<com::chancho::Category>("Food", com::chancho::Category::Type::INCOME);
    auto tran = std::make_shared<com::chancho::Transaction>(acc, 90.0, cat, "", "");
    auto book = std::make_shared<com::chancho::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto worker = std::make_shared<com::chancho::qml::workers::transactions::SingleStore>(book, tran->account,
        tran->category, tran->date, tran->amount, tran->contents, tran->memo);
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

QTEST_MAIN(com::chancho::qml::workers::transactions::tests::TestSingleStore)


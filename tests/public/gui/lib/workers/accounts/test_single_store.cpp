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
#include <com/chancho/qml/workers/accounts/single_store.h>

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

namespace accounts {

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
    QTest::addColumn<QString>("name");
    QTest::addColumn<double>("amount");
    QTest::addColumn<double>("initialAmount");
    QTest::addColumn<QString>("memo");
    QTest::addColumn<QString>("color");

    QTest::newRow("first-obj") << QUuid::createUuid().toString() << 3.4 << 3.4 << "Random memo" << "#2323";
    QTest::newRow("second-obj") << QUuid::createUuid().toString() << 90.0 << 90.0 << "Hello" << "#fff";
    QTest::newRow("last-obj") << QUuid::createUuid().toString() << 67.7 << 67.7 << "Bye" << "#000";
}

void
TestSingleStore::testRun() {
    QFETCH(QString, name);
    QFETCH(double, amount);
    QFETCH(double, initialAmount);
    QFETCH(QString, memo);
    QFETCH(QString, color);
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto expected = std::make_shared<com::chancho::Account>(name, amount, memo, color);
    expected->initialAmount = initialAmount;

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::AccountPtr>(AccountEquals(expected))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto worker = std::make_shared<com::chancho::qml::workers::accounts::SingleStore>(book, name, memo, color,
                                                                                      initialAmount);
    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failureSpy.count(), 0);
}

void
TestSingleStore::testRunBookError() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    QString name("Name");
    QString memo("Memo");
    QString color("Color");
    double initialAmount = 90;

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::AccountPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto worker = std::make_shared<com::chancho::qml::workers::accounts::SingleStore>(book, name, memo, color,
                                                                                      initialAmount);
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

QTEST_MAIN(com::chancho::qml::workers::accounts::tests::TestSingleStore)


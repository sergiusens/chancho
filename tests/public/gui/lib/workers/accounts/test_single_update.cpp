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
#include <com/chancho/qml/workers/accounts/single_update.h>

#include "book.h"
#include "matchers.h"

#include "test_single_update.h"

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
TestSingleUpdate::init() {
    BaseTestCase::init();
}

void
TestSingleUpdate::cleanup() {
    BaseTestCase::cleanup();
}

void
TestSingleUpdate::testRun_data() {
    QTest::addColumn<QPair<QString, QString>>("name");
    QTest::addColumn<QPair<double, double>>("amount");
    QTest::addColumn<QPair<double, double>>("initialAmount");
    QTest::addColumn<QPair<QString, QString>>("memo");
    QTest::addColumn<QPair<QString, QString>>("color");

    QTest::newRow("diff-name") << QPair<QString, QString>("oldName", "newName") << QPair<double, double>(9, 9)
            << QPair<double, double>(10, 10) << QPair<QString, QString>("memo", "memo")
            << QPair<QString, QString>("color", "color");
    QTest::newRow("diff-memo") << QPair<QString, QString>("name", "name") << QPair<double, double>(9, 9)
            << QPair<double, double>(10, 10) << QPair<QString, QString>("oldMemo", "newMemo")
            << QPair<QString, QString>("color", "color");
    QTest::newRow("diff-color") << QPair<QString, QString>("name", "name") << QPair<double, double>(9, 9)
            << QPair<double, double>(10, 10) << QPair<QString, QString>("memo", "memo")
            << QPair<QString, QString>("oldColor", "newColor");
}

void
TestSingleUpdate::testRun() {
    typedef QPair<QString, QString> stringPair;
    typedef QPair<double, double> doublePair;
    QFETCH(stringPair, name);
    QFETCH(doublePair, amount);
    QFETCH(doublePair, initialAmount);
    QFETCH(stringPair, memo);
    QFETCH(stringPair, color);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto acc = std::make_shared<com::chancho::Account>(name.first, amount.first, memo.first, color.first);
    acc->initialAmount = initialAmount.first;
    auto expected = std::make_shared<com::chancho::Account>(name.second, amount.second, memo.second, color.second);
    expected->initialAmount = initialAmount.second;

    //set the expectations
    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::AccountPtr>(AccountEquals(expected))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto worker = std::make_shared<com::chancho::qml::workers::accounts::SingleUpdate>(book, acc, name.second,
                                                                                       memo.second, color.second);

    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failureSpy.count(), 0);
}

void
TestSingleUpdate::testRunBookError() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    QString name("Name");
    QString memo("Memo");
    QString color("Color");
    auto acc = std::make_shared<com::chancho::Account>(name, 34, memo, color);

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::AccountPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto worker = std::make_shared<com::chancho::qml::workers::accounts::SingleUpdate>(book, acc, name + name, memo, color);
    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failureSpy.count(), 1);
}

void
TestSingleUpdate::testRunNoUpdate() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    QString name("Name");
    QString memo("Memo");
    QString color("Color");
    auto acc = std::make_shared<com::chancho::Account>(name, 34, memo, color);

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::AccountPtr>(_)))
            .Times(0);

    EXPECT_CALL(*book.get(), isError())
            .Times(0);

    auto worker = std::make_shared<com::chancho::qml::workers::accounts::SingleUpdate>(book, acc, name, memo, color);
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

QTEST_MAIN(com::chancho::qml::workers::accounts::tests::TestSingleUpdate)


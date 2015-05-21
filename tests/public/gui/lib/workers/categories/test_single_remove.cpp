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
#include <com/chancho/category.h>
#include <com/chancho/qml/workers/categories/single_remove.h>

#include "book.h"
#include "matchers.h"

#include "test_single_remove.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::Return;

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace categories {

namespace tests {

void
TestSingleRemove::init() {
    BaseTestCase::init();
}

void
TestSingleRemove::cleanup() {
    BaseTestCase::cleanup();
}

void
TestSingleRemove::testRun_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<com::chancho::Category::Type >("type");
    QTest::addColumn<QString>("color");

    QTest::newRow("income-obj") << QUuid::createUuid().toString() << com::chancho::Category::Type::INCOME << "#ccc";
    QTest::newRow("expense-obj") << QUuid::createUuid().toString() << com::chancho::Category::Type::EXPENSE << "#fff";
}

void
TestSingleRemove::testRun() {
    QFETCH(QString, name);
    QFETCH(com::chancho::Category::Type, type);
    QFETCH(QString, color);

    auto cat = std::make_shared<com::chancho::Category>(name, type, color);
    auto book = std::make_shared<com::chancho::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                remove(Matcher<com::chancho::CategoryPtr>(CategoryEquals(cat))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto worker = std::make_shared<com::chancho::qml::workers::categories::SingleRemove>(book, cat);
    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failureSpy.count(), 0);

}

void
TestSingleRemove::testRunBookError() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto cat = std::make_shared<com::chancho::Category>();

    EXPECT_CALL(*book.get(),
                remove(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto worker = std::make_shared<com::chancho::qml::workers::categories::SingleRemove>(book, cat);
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

QTEST_MAIN(com::chancho::qml::workers::categories::tests::TestSingleRemove)


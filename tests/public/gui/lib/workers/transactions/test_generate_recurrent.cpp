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

#include "test_generate_recurrent.h"
#include "../../../../../../src/public/gui/lib/com/chancho/qml/workers/transactions/generate_recurrent.h"

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
TestGenerateRecurrent::init() {
    BaseTestCase::init();
}

void
TestGenerateRecurrent::cleanup() {
    BaseTestCase::cleanup();
}

void
TestGenerateRecurrent::testRun() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                generateRecurrentTransactions())
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto worker = std::make_shared<com::chancho::qml::workers::transactions::GenerateRecurrent>(book);

    // ensure that the signals are indeed fired
    QSignalSpy successSpy(worker.get(), SIGNAL(success()));
    QSignalSpy failureSpy(worker.get(), SIGNAL(failure()));

    worker->run();

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failureSpy.count(), 0);
}

void
TestGenerateRecurrent::testRunBookError() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();

    EXPECT_CALL(*book.get(),
                generateRecurrentTransactions())
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto worker = std::make_shared<com::chancho::qml::workers::transactions::GenerateRecurrent>(book);

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

QTEST_MAIN(com::chancho::qml::workers::transactions::tests::TestGenerateRecurrent)

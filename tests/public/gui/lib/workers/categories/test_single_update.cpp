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
#include <com/chancho/qml/workers/categories/single_update.h>
#include <com/chancho/qml/book.h>

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

namespace categories {

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
    QTest::addColumn<QPair<QString, QString>>("color");
    QTest::addColumn<QPair<qml::Book::TransactionType, qml::Book::TransactionType>>("type");

    QTest::newRow("diff-name") << QPair<QString, QString>("oldName", "newName")
    << QPair<QString, QString>("color", "color")
    << QPair<qml::Book::TransactionType, qml::Book::TransactionType>(qml::Book::TransactionType::INCOME, qml::Book::TransactionType::INCOME);
    QTest::newRow("diff-color") << QPair<QString, QString>("name", "name")
    << QPair<QString, QString>("oldColor", "newColor")
    << QPair<qml::Book::TransactionType, qml::Book::TransactionType>(qml::Book::TransactionType::INCOME, qml::Book::TransactionType::INCOME);
    QTest::newRow("diff-type") << QPair<QString, QString>("name", "name")
    << QPair<QString, QString>("color", "color")
    << QPair<qml::Book::TransactionType, qml::Book::TransactionType>(qml::Book::TransactionType::INCOME, qml::Book::TransactionType::EXPENSE);
}

void
TestSingleUpdate::testRun() {
    typedef QPair<QString, QString> stringPair;
    typedef QPair<qml::Book::TransactionType, qml::Book::TransactionType> typePair;
    QFETCH(stringPair, name);
    QFETCH(stringPair, color);
    QFETCH(typePair, type);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    com::chancho::CategoryPtr cat;
    if (type.first == qml::Book::TransactionType::INCOME) {
        cat = std::make_shared<com::chancho::Category>(name.first, com::chancho::Category::Type::INCOME, color.first);
    } else {
        cat = std::make_shared<com::chancho::Category>(name.first, com::chancho::Category::Type::EXPENSE, color.first);
    }

    com::chancho::CategoryPtr expected;
    if (type.second == qml::Book::TransactionType::INCOME) {
        expected = std::make_shared<com::chancho::Category>(name.second, com::chancho::Category::Type::INCOME,
                                                            color.second);
    } else {
        expected = std::make_shared<com::chancho::Category>(name.second, com::chancho::Category::Type::EXPENSE,
                                                            color.second);
    }

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::CategoryPtr>(CategoryEquals(expected))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto worker = std::make_shared<com::chancho::qml::workers::categories::SingleUpdate>(book, cat, name.second,
                                                                                         color.second, type.second);
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
    QString color("Color");
    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::INCOME, color);

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto worker = std::make_shared<com::chancho::qml::workers::categories::SingleUpdate>(book, cat, name,
         color, qml::Book::TransactionType::EXPENSE);

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
    QString color("Color");
    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::INCOME, color);

    EXPECT_CALL(*book.get(),
                store(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(0);

    EXPECT_CALL(*book.get(), isError())
            .Times(0);

    auto worker = std::make_shared<com::chancho::qml::workers::categories::SingleUpdate>(book, cat, name,
                                                                                         color, qml::Book::TransactionType::INCOME);

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

QTEST_MAIN(com::chancho::qml::workers::categories::tests::TestSingleUpdate)


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
#include <com/chancho/qml/workers/categories/multi_store.h>
#include <com/chancho/qml/book.h>

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

namespace categories {

namespace tests {

namespace {
    const QString NAME_KEY = "name";
    const QString COLOR_KEY = "color";
    const QString TYPE_KEY = "type";
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
    QTest::addColumn<QVariantList>("categories");
    QTest::addColumn<QList<com::chancho::CategoryPtr>>("expected");

    QVariantList empty;
    QList<com::chancho::CategoryPtr> emptyExpected;

    QTest::newRow("empty-list") << empty << emptyExpected;

    QList<com::chancho::CategoryPtr> singleExpected;
    singleExpected.append(std::make_shared<com::chancho::Category>(QUuid::createUuid().toString(),
        com::chancho::Category::Type::INCOME));

    QVariantMap singleMap;
    singleMap[NAME_KEY] = singleExpected.at(0)->name;
    singleMap[TYPE_KEY] = qml::Book::TransactionType::INCOME;
    singleMap[COLOR_KEY] = singleExpected.at(0)->color;

    QVariantList singleCategory;
    singleCategory.append(singleMap);

    QTest::newRow("single-account-list") << singleCategory << singleExpected;

    QVariantList multiCategory;
    QList<com::chancho::CategoryPtr> multiExpected;

    for(int index = 0; index < 10; index++) {
        com::chancho::CategoryPtr cat;
        if (index < 2) {
            cat = std::make_shared<com::chancho::Category>(QUuid::createUuid().toString(),
                com::chancho::Category::Type::EXPENSE);
        } else {
            cat = std::make_shared<com::chancho::Category>(QUuid::createUuid().toString(),
                com::chancho::Category::Type::INCOME);
        }
        QVariantMap map;
        map[NAME_KEY] = cat->name;
        map[COLOR_KEY] = cat->color;
        map[TYPE_KEY] = (cat->type == com::chancho::Category::Type::INCOME)? qml::Book::INCOME : qml::Book::EXPENSE;
        multiCategory.append(map);
        multiExpected.append(cat);

    };

    QTest::newRow("multi-account-list") << multiCategory << multiExpected;
}

void
TestMultiStore::testRun() {
    QFETCH(QVariantList, categories );
    QFETCH(QList<com::chancho::CategoryPtr>, expected);
    auto book = std::make_shared<com::chancho::tests::MockBook>();

    // set the expectations
    EXPECT_CALL(*book.get(),
                store(Matcher<QList<com::chancho::CategoryPtr>>(CategoryListEquals(expected))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto worker = std::make_shared<com::chancho::qml::workers::categories::MultiStore>(book, categories);
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
    QVariantList cats;

    EXPECT_CALL(*book.get(),
                store(Matcher<QList<com::chancho::CategoryPtr>>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto worker = std::make_shared<com::chancho::qml::workers::categories::MultiStore>(book, cats);
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

QTEST_MAIN(com::chancho::qml::workers::categories::tests::TestMultiStore)

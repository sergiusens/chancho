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

#include <QVariant>
#include "test_category.h"

namespace chancho = com::chancho;

void
TestCategory::init() {
    BaseTestCase::init();

}

void
TestCategory::cleanup() {
    BaseTestCase::cleanup();
}

void
TestCategory::testNameTypeConstructor_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<chancho::Category::Type>("type");

    QTest::newRow("income-obj") << "Salary" << chancho::Category::Type::INCOME;
    QTest::newRow("expense-obj") << "Food" << chancho::Category::Type::EXPENSE;
}

void
TestCategory::testNameTypeConstructor() {
    QFETCH(QString, name);
    QFETCH(chancho::Category::Type, type);

    auto category = std::make_shared<chancho::Category>(name, type);
    QCOMPARE(category->name, name);
    QCOMPARE(category->type, type);
}

void
TestCategory::testNameTypeParentConstrutor_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<chancho::Category::Type>("type");
    QTest::addColumn<chancho::CategoryPtr>("parent");

    auto firstParent = std::make_shared<chancho::Category>("Salary", chancho::Category::Type::INCOME);
    auto secondParent = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);

    QTest::newRow("income-obj") << "Bonus" << chancho::Category::Type::INCOME << firstParent;
    QTest::newRow("expense-obj") << "Restaurant" << chancho::Category::Type::EXPENSE << secondParent;
}

void
TestCategory::testNameTypeParentConstrutor() {
    QFETCH(QString, name);
    QFETCH(chancho::Category::Type, type);
    QFETCH(chancho::CategoryPtr, parent);

    auto category = std::make_shared<chancho::Category>(name, type, parent);
    QCOMPARE(category->name, name);
    QCOMPARE(category->type, type);
    QCOMPARE(category->parent->name, parent->name);
    QCOMPARE(category->parent->type, parent->type);
}

void
TestCategory::testWasDbStored_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<chancho::Category::Type>("type");
    QTest::addColumn<QUuid>("dbId");
    QTest::addColumn<bool>("result");

    QTest::newRow("income-obj") << "Bonus" << chancho::Category::Type::INCOME << QUuid() << false;
    QTest::newRow("expense-obj") << "Restaurant" << chancho::Category::Type::EXPENSE << QUuid::createUuid() << true;
}

void
TestCategory::testWasDbStored() {
    QFETCH(QString, name);
    QFETCH(chancho::Category::Type, type);
    QFETCH(QUuid, dbId);
    QFETCH(bool, result);

    auto category = std::make_shared<PublicSetters>(name, type);
    category->_dbId = dbId;
    QCOMPARE(category->wasStoredInDb(), result);
}

QTEST_MAIN(TestCategory)
